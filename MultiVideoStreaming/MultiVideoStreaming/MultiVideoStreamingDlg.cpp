// MultiVideoStreamingDlg.cpp: 구현 파일

#include <cmath>
#include <chrono>

#include "pch.h"
#include "framework.h"
#include "MultiVideoStreaming.h"
#include "MultiVideoStreamingDlg.h"
#include "afxdialogex.h"

#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#pragma comment(lib, "opencv_world450d.lib")
#else
#pragma comment(lib, "opencv_world450.lib")
#endif

// CMultiVideoStreamingDlg 대화 상자
CMultiVideoStreamingDlg::CMultiVideoStreamingDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_MultiVideoStreaming_DIALOG, pParent)
{
	for (int i = 0; i < 4; ++i)
	{
		// 영상 스트리밍
		videoStreamingThreads[i] = nullptr;
		m_pCaptures[i] = nullptr;
		m_matImages[i] = nullptr;
		m_matImageTs[i] = nullptr;
		bitmaps[i] = nullptr;
		isVideoStreaming[i] = false;

		// UI
		isButtonPushed[i] = false;
	}

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMultiVideoStreamingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON0, videoStreamingStartStopBtn[0]);
	DDX_Control(pDX, IDC_BUTTON1, videoStreamingStartStopBtn[1]);
	DDX_Control(pDX, IDC_BUTTON2, videoStreamingStartStopBtn[2]);
	DDX_Control(pDX, IDC_BUTTON3, videoStreamingStartStopBtn[3]);
}

BEGIN_MESSAGE_MAP(CMultiVideoStreamingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND_RANGE(IDC_BUTTON0, IDC_BUTTON3, CMultiVideoStreamingDlg::OnBtnClicked)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CMultiVideoStreamingDlg 메시지 처리기
BOOL CMultiVideoStreamingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
		}
	}

	/* 이 대화 상자의 아이콘을 설정합니다.
	응용 프로그램의 주 창이 대화 상자가 아닐 경우에는 프레임워크가 이 작업을 자동으로 수행합니다. */
	SetIcon(m_hIcon, TRUE); // 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE); // 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	/********** INI 파일 정보 가져오기 **********/
	// ini 파일 열기 / 파일 읽을 때, 쓰는 변수 초기화
	wchar_t excuteProgramPath[256] = { 0, };
	GetModuleFileName(NULL, excuteProgramPath, sizeof(excuteProgramPath) / 2);
	excuteFolderPath = excuteProgramPath;
	excuteFolderPath = excuteFolderPath.Left(excuteFolderPath.ReverseFind(_T('\\')));
	CString iniFilePath = excuteFolderPath + "\\init.ini";
	wchar_t wcaReturn[1024] = { 0, };

	// Video Streaming path app name 읽기
	GetPrivateProfileString(_T("Video Streaming path"), _T("0"), _T(""), wcaReturn, 1024, iniFilePath);
	streamingAddresses[0] = wcaReturn;
	GetPrivateProfileString(_T("Video Streaming path"), _T("1"), _T(""), wcaReturn, 1024, iniFilePath);
	streamingAddresses[1] = wcaReturn;
	GetPrivateProfileString(_T("Video Streaming path"), _T("2"), _T(""), wcaReturn, 1024, iniFilePath);
	streamingAddresses[2] = wcaReturn;
	GetPrivateProfileString(_T("Video Streaming path"), _T("3"), _T(""), wcaReturn, 1024, iniFilePath);
	streamingAddresses[3] = wcaReturn;

	return TRUE; // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

/* 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 아래 코드가 필요합니다. 
문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는 프레임워크에서 이 작업을 자동으로 수행합니다.*/
void CMultiVideoStreamingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서 이 함수를 호출합니다.
HCURSOR CMultiVideoStreamingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMultiVideoStreamingDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMultiVideoStreamingDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	for (int i = 0; i < 4; ++i)
	{
		if (m_pCaptures[i] != nullptr)
		{
			// memory leak이 있다.
			m_pCaptures[i]->release();
			delete m_pCaptures[i];
			m_pCaptures[i] = nullptr;
		}

		if (m_matImages[i] != nullptr)
		{
			// memory leak이 있다.
			m_matImages[i]->release();
			delete m_matImages[i];
			m_matImages[i] = nullptr;
		}

		if (m_matImageTs[i] != nullptr)
		{
			// memory leak이 있다.
			m_matImageTs[i]->release();
			delete m_matImageTs[i];
			m_matImageTs[i] = nullptr;
		}

		isButtonPushed[i] = false;
	}

	CDialogEx::OnClose();
}

BOOL CMultiVideoStreamingDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::DestroyWindow();
}

void CMultiVideoStreamingDlg::OnBtnClicked(unsigned int _ctrlID)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CString btnStr;
	int threadNum = _ctrlID - IDC_BUTTON0;

	/********** 실행 상태 **********/
	if (isButtonPushed[threadNum] == false)
	{
		// 버튼 상태
		isButtonPushed[threadNum] = true;

		// 버튼 문자열
		btnStr.Format(_T("%d번 영상 Stop"), threadNum);
		videoStreamingStartStopBtn[threadNum].SetWindowTextW(btnStr);

		// 비디오 스트리밍 시작
		InitVideoStreaming(threadNum);
		isVideoStreaming[threadNum] = true;
		videoStreamingThreads[threadNum] = new std::thread(&CMultiVideoStreamingDlg::VideoStreaming, this, threadNum);
	}
	else
	{
		// 비디오 스트리밍 종료
		isVideoStreaming[threadNum] = false;

		// 버튼 상태
		isButtonPushed[threadNum] = false;

		// 버튼 문자열
		btnStr.Format(_T("%d번 영상 Start"), threadNum);
		videoStreamingStartStopBtn[threadNum].SetWindowTextW(btnStr);
	}
}

void CMultiVideoStreamingDlg::CreateBitmapInfo(int _threadNum, int w, int h, int bpp)
{
	if (bitmaps[_threadNum] != nullptr)
	{
		delete bitmaps[_threadNum];
		bitmaps[_threadNum] = nullptr;
	}

	if (bpp == 8)
		bitmaps[_threadNum] = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		bitmaps[_threadNum] = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];

	bitmaps[_threadNum]->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmaps[_threadNum]->bmiHeader.biPlanes = 1;
	bitmaps[_threadNum]->bmiHeader.biBitCount = bpp;
	bitmaps[_threadNum]->bmiHeader.biCompression = BI_RGB;
	bitmaps[_threadNum]->bmiHeader.biSizeImage = 0;
	bitmaps[_threadNum]->bmiHeader.biXPelsPerMeter = 0;
	bitmaps[_threadNum]->bmiHeader.biYPelsPerMeter = 0;
	bitmaps[_threadNum]->bmiHeader.biClrUsed = 0;
	bitmaps[_threadNum]->bmiHeader.biClrImportant = 0;

	if (bpp == 8)
	{
		for (int i = 0; i < 256; ++i)
		{
			bitmaps[_threadNum]->bmiColors[i].rgbBlue = (BYTE)i;
			bitmaps[_threadNum]->bmiColors[i].rgbGreen = (BYTE)i;
			bitmaps[_threadNum]->bmiColors[i].rgbRed = (BYTE)i;
			bitmaps[_threadNum]->bmiColors[i].rgbReserved = 0;
		}
	}

	bitmaps[_threadNum]->bmiHeader.biWidth = w;
	bitmaps[_threadNum]->bmiHeader.biHeight = -h;
}

void CMultiVideoStreamingDlg::DrawImage(int _threadNum)
{
	CreateBitmapInfo(_threadNum, m_matImages[_threadNum]->cols, m_matImages[_threadNum]->rows, m_matImages[_threadNum]->channels() * 8);

	CClientDC dc(GetDlgItem(IDC_PICTURE0 + _threadNum));

	CRect rect;
	GetDlgItem(IDC_PICTURE0 + _threadNum)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_matImages[_threadNum]->cols, m_matImages[_threadNum]->rows, m_matImages[_threadNum]->data, bitmaps[_threadNum], DIB_RGB_COLORS, SRCCOPY);
}

void CMultiVideoStreamingDlg::DrawImageBitblt(int _threadNum)
{
	CClientDC dc(GetDlgItem(IDC_PICTURE0 + _threadNum));

	CRect rect;
	GetDlgItem(IDC_PICTURE0 + _threadNum)->GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	cv::resize(*m_matImages[_threadNum], *m_matImageTs[_threadNum], cv::Size(960, 540));

	for (int col = 0; col < 540; ++col)
	{
		for (int row = 0; row < 960; ++row)
		{
			bmpBufferes[_threadNum][col * 960 * 4 + row * 4] = m_matImageTs[_threadNum]->data[col * 960 * 3 + row * 3];
			bmpBufferes[_threadNum][col * 960 * 4 + row * 4 + 1] = m_matImageTs[_threadNum]->data[col * 960 * 3 + row * 3 + 1];
			bmpBufferes[_threadNum][col * 960 * 4 + row * 4 + 2] = m_matImageTs[_threadNum]->data[col * 960 * 3 + row * 3 + 2];
			bmpBufferes[_threadNum][col * 960 * 4 + row * 4 + 3] = 255;
		}
	}

	CBitmap bitmap;
	BOOL re = bitmap.CreateBitmap(960, 540, 1, 32, bmpBufferes[_threadNum]);
	memDC.SelectObject(bitmap);

	BOOL re2 = dc.BitBlt(0, 0, 960, 540, &memDC, 0, 0, SRCCOPY);
}

void CMultiVideoStreamingDlg::InitVideoStreaming(int _threadNum)
{
	m_pCaptures[_threadNum] = new cv::VideoCapture(((CStringA)streamingAddresses[_threadNum]).GetString());
	if (m_pCaptures[_threadNum] == nullptr) return;

	m_matImages[_threadNum] = new cv::Mat();
	if (m_matImages[_threadNum] == nullptr) return;

	m_matImageTs[_threadNum] = new cv::Mat();
	if (m_matImageTs[_threadNum] == nullptr) return;
}

void CMultiVideoStreamingDlg::VideoStreaming(int _threadNum)
{
	std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point nowFrameTime;
	std::chrono::duration<double> timeSpan;

	// FPS 변수
	double fps = 0.0;
	CString cStrFPS;
	std::string strFPS;
	int frameCount = 0;

	// Debug
	CString fileName;
	fileName.Format(_T("%s\\VideoStreamingLog%d"), excuteFolderPath.GetString(), _threadNum);
	FILE* logFile = nullptr;
	fopen_s(&logFile, (CStringA)fileName, "a");

	// 화면 랜더링
	bmpBufferes[_threadNum] = new unsigned char[2073600];

	while (isVideoStreaming[_threadNum] == true)
	{
		nowFrameTime = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(nowFrameTime - lastFrameTime);
		// if (timeSpan.count() < 0.1) continue; // XX초 보다 시간이 작으면 좀 더 기다림
		if (m_pCaptures[_threadNum]->read(*m_matImages[_threadNum]) == false)
		{
			Logging(logFile, _T("read false return"));
			m_pCaptures[_threadNum]->release();
			m_pCaptures[_threadNum] = new cv::VideoCapture(((CStringA)streamingAddresses[_threadNum]).GetString());
			continue;
		}
		
		if (m_matImages[_threadNum]->empty() == true) continue;

		/********** FPS 화면 출력 **********/
		if (timeSpan.count() > 1.0)
		{
			fps = frameCount;
			cStrFPS.Format(_T("%d - %.1lf"), _threadNum, fps);
			strFPS = CT2CA(cStrFPS);
			lastFrameTime = nowFrameTime;
			frameCount = 0;
			TRACE(_T("fps : ") + cStrFPS + _T("\n"));
		}
		putText(*m_matImages[_threadNum], strFPS, cv::Point(100, 100), 0, 2, cv::Scalar(255, 0, 0), 3);

		/********** 화면에 이미지 그리기 **********/
		DrawImageBitblt(_threadNum);
		// DrawImage(_threadNum);

		// lastFrameTime = nowFrameTime;
		frameCount++;
	}

	/********** 영상이 멈추면 화면을 초기화 **********/
	if (m_matImages[_threadNum] != nullptr)
	{
		for (int v = 0; v < m_matImages[_threadNum]->rows; ++v)
		{
			for (int u = 0; u < m_matImages[_threadNum]->cols; ++u)
			{
				m_matImages[_threadNum]->data[v * m_matImages[_threadNum]->cols * 3 + u * 3 + 0] = 240;
				m_matImages[_threadNum]->data[v * m_matImages[_threadNum]->cols * 3 + u * 3 + 1] = 240;
				m_matImages[_threadNum]->data[v * m_matImages[_threadNum]->cols * 3 + u * 3 + 2] = 240;
			}
		}
	}
	DrawImage(_threadNum);

	m_pCaptures[_threadNum]->release();
	m_pCaptures[_threadNum] = nullptr;
	m_matImages[_threadNum]->release();
	m_matImages[_threadNum] = nullptr;
	m_matImageTs[_threadNum]->release();
	m_matImageTs[_threadNum] = nullptr;

	delete bmpBufferes[_threadNum];
	bmpBufferes[_threadNum] = nullptr;

	// Debug
	if (logFile != nullptr)
		fclose(logFile);
}

void CMultiVideoStreamingDlg::Logging(FILE* _file, CString _csMemo)
{
	SYSTEMTIME nowSysTime;
	CString csLog;
	GetLocalTime(&nowSysTime);
	csLog.Format(_T("%d-%02d-%02d %02d:%02d:%02d.%03d %s\n"),
		nowSysTime.wYear,
		nowSysTime.wMonth,
		nowSysTime.wDay,
		nowSysTime.wHour,
		nowSysTime.wMinute,
		nowSysTime.wSecond,
		nowSysTime.wMilliseconds,
		_csMemo.GetString());
	if (_file != nullptr) fputs((CStringA)csLog, _file);
}