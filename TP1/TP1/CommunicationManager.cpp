#include "pch.h"
#include "CommunicationManager.h"

#include <afxinet.h>

CommunicationManager::CommunicationManager()
{
	pIsRtspStreaming = nullptr;

	communicationStatus = false;

	isSocketConnected = false;

	for (int thc = 0 /* Thread count */; thc < 4; ++thc)
	{
		sendBuf[thc][0] = 1; // IF_SC100.Header = 1
		for (int i = 1; i < 4096; ++i)
		{
			sendBuf[thc][i] = 0;
		}

		resultSize[thc] = 0;

		getPTZThreads[thc] = nullptr;

		ptz[thc][0] = 0.0;
		ptz[thc][1] = 0.0;
		ptz[thc][2] = 0.0;

		// CCTV index
		cctvIndex[thc] = -1;
	}

	ptz[0][0] = 33000;
	ptz[0][1] = 7000;
	ptz[1][0] = 32000;
	ptz[1][1] = 7000;
	ptz[2][0] = 938;
	ptz[2][1] = 6913;
	ptz[3][0] = 13500;
	ptz[3][1] = 7000;
}

CommunicationManager::~CommunicationManager()
{
	m_Socket.Close();
}

void CommunicationManager::SetTime(int _thn)
{
	if (communicationStatus == false) return;

	memcpy(&sendBuf[_thn][33], (unsigned char*)(LPCTSTR)GetNowTime(), 54);
}

void CommunicationManager::SetSize(int _thn)
{
	if (communicationStatus == false) return;

	sendBuf[_thn][87] = static_cast<unsigned char>(resultSize[_thn]);
}

void CommunicationManager::SetRepetitionData(
	int _thn, int _ID, int _objType, float _XAxis, float _ZAxis,
	int _left, int _right, int _top, int _bottom, float _percent)
{
	if (communicationStatus == false) return;

	CString csID;
	csID.Format(_T("%s %02d 00000"), csDay, cctvIndex[_thn]); // csID.Format(_T("%s %02d %05d"), csDay, cctvIndex[_thn], _ID);
	memcpy(&repetitionData[_thn].ID, (unsigned char*)(LPCTSTR)csID, 40);
	repetitionData[_thn].objType = 26; // _objType
	repetitionData[_thn].XAxis = _XAxis;
	repetitionData[_thn].ZAxis = _ZAxis;
	repetitionData[_thn].objX1 = _left;
	repetitionData[_thn].objY1 = _top;
	repetitionData[_thn].objX2 = _right;
	repetitionData[_thn].objY2 = _bottom;
	repetitionData[_thn].percent = _percent;

	memcpy(&sendBuf[_thn][88 + resultSize[_thn] * sizeof(repetitionData[_thn])], &repetitionData[_thn], sizeof(repetitionData[_thn]));
	resultSize[_thn]++;
}

bool CommunicationManager::TryConnectServer()
{
	if (isSocketConnected == true) return isSocketConnected;

	// 소켓 생성
	m_Socket.Create();

	// SC server 연결
	if (m_Socket.Connect(serverIPStr, 6097) == FALSE)
	{
		m_Socket.Close();
		isSocketConnected = false;
	}
	else
	{
		isSocketConnected = true;

		for (int thn = 0; thn < 4; ++thn)
		{
			memcpy_s(&sendBuf[thn][1], 32, cctvIPs[thn], static_cast<__int64>(cctvIPs[thn].GetLength()) * 2);
		}
	}

	return isSocketConnected;
}

CString CommunicationManager::GetNowTime()
{
	CString strNowYMDHMSM;
	SYSTEMTIME nowSysTime;
	GetLocalTime(&nowSysTime);

	tm today0H0M0S;
	today0H0M0S.tm_year = nowSysTime.wYear - 1900;
	today0H0M0S.tm_mon = nowSysTime.wMonth - 1;
	today0H0M0S.tm_mday = nowSysTime.wDay;
	today0H0M0S.tm_hour = 0;
	today0H0M0S.tm_min = 0;
	today0H0M0S.tm_sec = 0;

	std::time_t timet0H0M0S = std::mktime(&today0H0M0S);
	std::chrono::system_clock::time_point timePoint0H0M0S = std::chrono::system_clock::from_time_t(timet0H0M0S);
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	__int64 microSec = std::chrono::duration_cast<std::chrono::microseconds>(now - timePoint0H0M0S).count() % 1000;

	strNowYMDHMSM.Format(_T("%d-%02d-%02d %02d:%02d:%02d.%03d%03d"),
		nowSysTime.wYear,
		nowSysTime.wMonth,
		nowSysTime.wDay,
		nowSysTime.wHour,
		nowSysTime.wMinute,
		nowSysTime.wSecond,
		nowSysTime.wMilliseconds,
		static_cast<int>(microSec)
	);

	csDay.Format(_T("%d-%02d-%02d "),
		nowSysTime.wYear,
		nowSysTime.wMonth,
		nowSysTime.wDay
	);

	return strNowYMDHMSM;
}

void CommunicationManager::SendPacket(int _thn)
{
	if (communicationStatus == false) return;

	int sendRtn = m_Socket.Send(sendBuf[_thn], 88 + 69 * resultSize[_thn]);

	if (sendRtn == SOCKET_ERROR)
	{
		isSocketConnected = false;

		TryConnectServer();
	}
}

void CommunicationManager::SocketClose()
{
	isSocketConnected = false;

	m_Socket.Close();
}

void CommunicationManager::StartGetPTZ(int _thn)
{
	getPTZThreads[_thn] = new std::thread(&CommunicationManager::RequestHTTP, this, _thn);
}

void CommunicationManager::RequestHTTP(int _thn)
{
	// Http web api 변수
	CInternetSession session;
	CString strHeader = _T("Authorization: Basic YWRtaW46YWRtaW4="); // Base64 to ASCII encoding 필요
	CHttpConnection* pConnection = nullptr;
	CHttpFile* pHTTPFile = nullptr;
	int bResult = 0;
	char* pszBuff = new char[1025];
	CString csPTZ;

	// 문자열 결과를 정수형 값으로 변경할 때 사용하는 변수
	CString csPanTemp, csTiltTemp, csZoomTemp, csPan, csTilt, csZoom;

	// FPS 변수
	std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point nowFrameTime;
	std::chrono::duration<double> timeSpan;
	int mili = 0;

	while (pIsRtspStreaming[_thn] == true)
	{
		nowFrameTime = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(nowFrameTime - lastFrameTime);
		if (timeSpan.count() < 0.1)
		{
			mili = 100 - static_cast<int>(timeSpan.count() * 1000.0);
			std::this_thread::sleep_for(std::chrono::milliseconds(mili));
			continue;
		}
		csPTZ = "";
		try
		{
			// Http web api 호출
			pConnection = (CHttpConnection*)session.GetHttpConnection(cctvIPs[_thn], (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);
			if (pConnection == nullptr) continue;
			pHTTPFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/cgi-bin/param.cgi?action=list&group=PTZPOS&channel=0"));
			if (pHTTPFile == nullptr) continue;
			pHTTPFile->SendRequest(strHeader, 0, 0);
			if (bResult == 0) continue;

			int nRead = 0;
			while ((nRead = pHTTPFile->Read(pszBuff, 1024)) > 0)
			{
				pszBuff[nRead] = 0;
				csPTZ += pszBuff;
			};

			// 결과로 나온 csPTZ 문자열에서 Pan, Tilt, Zoom 값 추출
			AfxExtractSubString(csPanTemp, csPTZ, 1, _T('='));
			AfxExtractSubString(csTiltTemp, csPTZ, 2, _T('='));
			AfxExtractSubString(csZoomTemp, csPTZ, 3, _T('='));
			AfxExtractSubString(csPan, csPanTemp, 0, _T('\r'));
			AfxExtractSubString(csTilt, csTiltTemp, 0, _T('\r'));
			AfxExtractSubString(csZoom, csZoomTemp, 0, _T('\r'));
			ptz[_thn][0] = _wtoi(csPan);
			ptz[_thn][1] = _wtoi(csTilt);
			ptz[_thn][2] = _wtoi(csZoom);
		}
		catch (CInternetException* pEx)
		{
			pConnection = nullptr;
			pEx->Delete();
		}

		lastFrameTime = nowFrameTime;
	}

	delete[] pszBuff;
	pHTTPFile->Close();
	delete pHTTPFile;
	if (pConnection != nullptr) pConnection->Close();
	delete pConnection;
	session.Close();
}

