// TP1SingleViewDlg.cpp: 구현 파일

#include <cmath>
#include <chrono>

#include "pch.h"
#include "framework.h"
#include "TP1SingleView.h"
#include "TP1SingleViewDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "IDManager.h"

#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#pragma comment(lib, "opencv_world450d.lib")
#pragma comment(lib, "yolo_cpp_dlld.lib")
#else
#pragma comment(lib, "opencv_world450.lib")
#pragma comment(lib, "yolo_cpp_dll.lib")
#endif

Detector* yoloDetector = nullptr;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) { CDialogEx::DoDataExchange(pDX); }

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CTP1SingleViewDlg 대화 상자
CTP1SingleViewDlg::CTP1SingleViewDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_TP1SINGLEVIEW_DIALOG, pParent)
{
	// Thread
	yoloThreads = nullptr;
	isYoloThreadRun = false;

	// Streaming video
	videoStreamingThreads = nullptr;
	m_pCaptures = nullptr;
	captureImage[0] = nullptr;
	captureImage[1] = nullptr;
	smallImage = nullptr;
	bitmaps = nullptr;
	isRtspStreaming = false;

	// Yolo State
	isInferencing = false;

	// Undistortion
	isUndistortion = 1;

	// ID tracking
	trackingMaxDistance = 40;
	
	// PTZ
	getPTZThreads = nullptr;
	ptz[0] = 0.0;
	ptz[1] = 0.0;
	ptz[2] = 0.0;

	// Location etimation
	latitude = 0.0;
	longitude = 0.0;
	cctvTMNorth = 0.0;
	cctvTMEast = 0.0;
	offsetX = 0.0;
	offsetY = 0.0;
	offsetZ = 0.0;
	cameraHeight = 0.0;

	// UI
	isButtonPushed = false;
	isNetBtnPushed = false;

	// Network
	communicationStatus = false;

	// Debug
	fileOpenError = 0;
	fputsError = 0;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTP1SingleViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, serverIP);
	DDX_Control(pDX, IDC_LIST_YOLO0, listYolo);
	DDX_Control(pDX, IDC_BUTTON0, yoloStartStopBtn);
	DDX_Control(pDX, IDC_NETWORKBUTTON, networkStartStopBtn);
}

BEGIN_MESSAGE_MAP(CTP1SingleViewDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON0, &CTP1SingleViewDlg::OnBtnClicked)
	ON_BN_CLICKED(IDC_NETWORKBUTTON, &CTP1SingleViewDlg::OnBtnClickedNetworkbutton)
END_MESSAGE_MAP()

// CTP1SingleViewDlg 메시지 처리기
BOOL CTP1SingleViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

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
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	/* 이 대화 상자의 아이콘을 설정합니다.
	응용 프로그램의 주 창이 대화 상자가 아닐 경우에는 프레임워크가 이 작업을 자동으로 수행합니다. */
	SetIcon(m_hIcon, TRUE); // 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE); // 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	/* 전체화면 만드는 코드
	LONG style = GetWindowLong(this->m_hWnd, GWL_STYLE);
	ShowWindow(SW_MAXIMIZE);
	style = GetWindowLong(this->m_hWnd, GWL_STYLE);
	style &= ~(WS_DLGFRAME | WS_THICKFRAME);
	SetWindowLong(this->m_hWnd, GWL_STYLE, style);
	*/

	/********** INI 파일 정보 가져오기 **********/
	// ini 파일 열기 / 파일 읽을 때, 쓰는 변수 초기화
	wchar_t excuteProgramPath[256] = { 0, };
	GetModuleFileName(NULL, excuteProgramPath, sizeof(excuteProgramPath) / 2);
	excuteFolderPath = excuteProgramPath;
	excuteFolderPath = excuteFolderPath.Left(excuteFolderPath.ReverseFind(_T('\\')));
	CString iniFilePath = excuteFolderPath + "\\init.ini";
	int wcaReturnSize = 1024;
	wchar_t* wcaReturn = new wchar_t[wcaReturnSize];

	// Read CCTV Streaming path app name
	GetPrivateProfileString(_T("CCTV Streaming path"), _T("0"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	streamingAddresses = wcaReturn;

	int curPos = 0;
	if (streamingAddresses.Left(4) != _T("rtsp"))
	{
		isRtspStreaming = false;
		cctvIPs.Format(_T("192.168.0.%d"), 100);
	}
	else
	{
		isRtspStreaming = true;
		streamingAddresses.Tokenize(_T("@"), curPos);
		cctvIPs = streamingAddresses.Tokenize(_T(":"), curPos);
	}

	// Read Yolo app name
	std::wstring wsReturn;
	std::string cfgFileName;
	GetPrivateProfileString(_T("Yolo"), _T("cfg"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	wsReturn = wcaReturn;
	cfgFileName.assign(wsReturn.begin(), wsReturn.end());
	std::string weightsFileName;
	GetPrivateProfileString(_T("Yolo"), _T("weights"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	wsReturn = wcaReturn;
	weightsFileName.assign(wsReturn.begin(), wsReturn.end());
	std::wstring namesFileName;
	GetPrivateProfileString(_T("Yolo"), _T("names"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	namesFileName = wcaReturn;

	// Read Network app name
	CString csDefaultIP, csTemp;
	int octet0, octet1, octet2, octet3 = 0;
	GetPrivateProfileString(_T("Network"), _T("DefaultIP"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	csDefaultIP = wcaReturn;
	curPos = 0;
	csTemp = csDefaultIP.Tokenize(_T("."), curPos);
	octet0 = _ttoi(csTemp);
	csTemp = csDefaultIP.Tokenize(_T("."), curPos);
	octet1 = _ttoi(csTemp);
	csTemp = csDefaultIP.Tokenize(_T("."), curPos);
	octet2 = _ttoi(csTemp);
	csTemp = csDefaultIP.Tokenize(_T("."), curPos);
	octet3 = _ttoi(csTemp);
	GetPrivateProfileString(_T("Network"), _T("CommunicationStatus"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	CString csCommunicationStatus = wcaReturn;
	if (csCommunicationStatus == _T("True") || csCommunicationStatus == _T("true"))
		communicationStatus = true;

	if (communicationStatus == false)
	{
		serverIP.EnableWindow(0); // Server IP 입력 창 비활성화
		networkStartStopBtn.EnableWindow(0); // 서버 연결 버튼 비활성화
		yoloStartStopBtn.EnableWindow(); // yolo start 버튼 활성화
	}

	// Read Undistortion app name
	GetPrivateProfileString(_T("Undistortion"), _T("state"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	CString csUndistortionStatus = wcaReturn;
	if (csUndistortionStatus != _T("True") && csUndistortionStatus != _T("true"))
		isUndistortion = 0;

	// Read ID tracking app name
	GetPrivateProfileString(_T("ID Tracking"), _T("trackingMaxDistance"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	trackingMaxDistance = _wtoi(wcaReturn);

	// Read LocationEstimation app name
	wchar_t* wcsStop = nullptr;
	GetPrivateProfileString(_T("LocationEstimation"), _T("latitude"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	latitude = wcstod(wcaReturn, &wcsStop);
	GetPrivateProfileString(_T("LocationEstimation"), _T("longitude"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	longitude = wcstod(wcaReturn, &wcsStop);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetX"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	offsetX = wcstod(wcaReturn, &wcsStop);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetY"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	offsetY = wcstod(wcaReturn, &wcsStop);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetZ"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	offsetZ = wcstod(wcaReturn, &wcsStop);
	GetPrivateProfileString(_T("LocationEstimation"), _T("cameraHeight"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
	cameraHeight = wcstod(wcaReturn, &wcsStop);
	TM cctvLocationTM = geoChange.GeotoTM(latitude, longitude, cameraHeight);
	cctvTMEast = cctvLocationTM.X;
	cctvTMNorth = cctvLocationTM.Y;
	if (isRtspStreaming == false)
	{
		GetPrivateProfileString(_T("LocationEstimation"), _T("pan"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
		ptz[0] = wcstod(wcaReturn, &wcsStop);
		GetPrivateProfileString(_T("LocationEstimation"), _T("tilt"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
		ptz[1] = wcstod(wcaReturn, &wcsStop);
		GetPrivateProfileString(_T("LocationEstimation"), _T("zoom"), _T(""), wcaReturn, wcaReturnSize, iniFilePath);
		ptz[2] = wcstod(wcaReturn, &wcsStop);
	}

	/********** 네트워크 **********/
	// IP 주소 입력 창 초기화
	serverIP.SetAddress(octet0, octet1, octet2, octet3);

	/********** Yolo detector 생성 **********/
	yoloDetector = new Detector(cfgFileName, weightsFileName);

	/********** Class 목록 가져오기 **********/
	CFile classNameFile;
	CString csFileBuffer; // 문자열 한줄씩 임시로 저장할 변수
	CStringA csaFileBuffer;
	int classNameFileSize = 0;
	CString csToken;

	classNameFile.Open(namesFileName.c_str(), CFile::modeRead);
	classNameFileSize = classNameFile.GetLength();
	classNameFileSize = classNameFile.Read(csaFileBuffer.GetBuffer(classNameFileSize), classNameFileSize);
	csaFileBuffer.ReleaseBuffer(classNameFileSize);
	classNameFile.Close();
	csFileBuffer = CA2T(csaFileBuffer, CP_UTF8);
	curPos = 0;
	while (true)
	{
		csToken = csFileBuffer.Tokenize(_T("\n"), curPos);
		if (csToken == "") break;
		classNames.push_back(csToken);
	}

	/********** 테이블 항목 초기화 **********/
	listYolo.InsertColumn(0, _T("No"), LVCFMT_CENTER, 40);
	listYolo.InsertColumn(1, _T("T ID"), LVCFMT_CENTER, 40);
	listYolo.InsertColumn(2, _T("Object Name"), LVCFMT_LEFT, 130);
	listYolo.InsertColumn(3, _T("Left"), LVCFMT_LEFT, 60);
	listYolo.InsertColumn(4, _T("Top"), LVCFMT_LEFT, 60);
	listYolo.InsertColumn(5, _T("Right"), LVCFMT_LEFT, 60);
	listYolo.InsertColumn(6, _T("Bottom"), LVCFMT_LEFT, 60);
	listYolo.InsertColumn(7, _T("Latitude"), LVCFMT_LEFT, 80);
	listYolo.InsertColumn(8, _T("Longitude"), LVCFMT_LEFT, 80);
	listYolo.InsertColumn(9, _T("Prob"), LVCFMT_LEFT, 100);
	listYolo.InsertColumn(10, _T("Local X"), LVCFMT_LEFT, 80);
	listYolo.InsertColumn(11, _T("Local Z"), LVCFMT_LEFT, 80);
	listYolo.SetExtendedStyle(LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	delete[] wcaReturn;

	return TRUE; // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTP1SingleViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

/* 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 아래 코드가 필요합니다. 
문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는 프레임워크에서 이 작업을 자동으로 수행합니다.*/
void CTP1SingleViewDlg::OnPaint()
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
HCURSOR CTP1SingleViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTP1SingleViewDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	delete yoloDetector;
	yoloDetector = nullptr;
}

void CTP1SingleViewDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_pCaptures != nullptr)
	{
		// memory leak이 있다.
		m_pCaptures->release();
		delete m_pCaptures;
		m_pCaptures = nullptr;
	}

	if (captureImage[0] != nullptr)
	{
		// memory leak이 있다.
		captureImage[0]->release();
		delete captureImage[0];
		captureImage[0] = nullptr;
	}

	if (captureImage[1] != nullptr)
	{
		// memory leak이 있다.
		captureImage[1]->release();
		delete captureImage[1];
		captureImage[1] = nullptr;
	}

	if (smallImage != nullptr)
	{
		// memory leak이 있다.
		smallImage->release();
		delete smallImage;
		smallImage = nullptr;
	}

	isButtonPushed = false;

	m_Socket.Close();

	CDialogEx::OnClose();
}

BOOL CTP1SingleViewDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::DestroyWindow();
}

void CTP1SingleViewDlg::OnBtnClickedNetworkbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (isNetBtnPushed == false)
	{
		isNetBtnPushed = true;

		// 서버 IP 가져오기
		serverIP.GetWindowText(serverIPStr);

		// 서버 연결
		if (TryConnectServer() == false) return;

		/********** UI **********/
		// 연결되어있는 동안 IP 주소 변경 못하게 하기
		serverIP.EnableWindow(0);

		// Yolo 시작 가능
		yoloStartStopBtn.EnableWindow();
		
		// 버튼 UI 글자 변경
		networkStartStopBtn.SetWindowTextW(_T("SC 연결 종료"));
	}
	else
	{
		isNetBtnPushed = false;

		/********** 네트워크 인터페이스 **********/

		// 소켓 닫기
		m_Socket.isSocketConnected = false;
		m_Socket.Close();
		
		/********** UI **********/
		// IP 주소 변경 할 수 있게 하기
		serverIP.EnableWindow();

		// Yolo 시작 못하게 하기
		yoloStartStopBtn.EnableWindow(0);

		// 버튼 UI 글자 변경
		networkStartStopBtn.SetWindowTextW(_T("SC 연결 시작"));
	}
}

void CTP1SingleViewDlg::OnBtnClicked()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (communicationStatus == true) // 서버 연결 함
	{
		if (isButtonPushed == false) // Inference 킴
		{
			// 버튼 상태
			isButtonPushed = true;

			// 하나라도 실행 중이면 네트워크 연결 종료 불가
			networkStartStopBtn.EnableWindow(0);

			// UI
			yoloStartStopBtn.SetWindowTextW(_T("Yolo Stop"));

			// 객체 감지 및 위치 추정 시작
			isYoloThreadRun = true;
			yoloThreads = new std::thread(&CTP1SingleViewDlg::InferenceStart, this);

			// PTZ 상태 값 수신 시작
			if (isRtspStreaming == true)
				getPTZThreads = new std::thread(&CTP1SingleViewDlg::RequestHTTP, this);
		}
		else // Inference 끔
		{
			// 버튼 상태
			isButtonPushed = false;

			// PTZ 값을 가져오고 있었다면 중지
			if (isRtspStreaming == true)
				isRtspStreaming = false;

			// Inference가 종료되었으면 네트워크 연결 종료 가능
			if (isButtonPushed == false) networkStartStopBtn.EnableWindow();

			// UI
			yoloStartStopBtn.SetWindowTextW(_T("Yolo Start"));

			// 객체 감지 및 위치 추정 종료
			InferenceStop();
		}
	}
	else // 서버 연결 안 함
	{
		if (isButtonPushed == false) // Inference 킴
		{
			isButtonPushed = true;
			
			// 버튼 텍스트 변경
			yoloStartStopBtn.SetWindowTextW(_T("Yolo Stop"));

			// 객체 감지 및 위치 추정 시작
			isYoloThreadRun = true;
			yoloThreads = new std::thread(&CTP1SingleViewDlg::InferenceStart, this);

			// PTZ 상태 값 수신 시작
			if (isRtspStreaming == true)
				getPTZThreads = new std::thread(&CTP1SingleViewDlg::RequestHTTP, this);
		}
		else // Inference 끔
		{
			isButtonPushed = false;

			// 버튼 텍스트 변경
			yoloStartStopBtn.SetWindowTextW(_T("Yolo Start"));

			// 객체 감지 및 위치 추정 종료
			InferenceStop();
		}
	}
	
}

void CTP1SingleViewDlg::CreateBitmapInfo(int _w, int _h, int _bpp)
{
	if (bitmaps != nullptr)
	{
		delete bitmaps;
		bitmaps = nullptr;
	}

	if (_bpp == 8)
		bitmaps = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		bitmaps = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];

	bitmaps->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmaps->bmiHeader.biPlanes = 1;
	bitmaps->bmiHeader.biBitCount = _bpp;
	bitmaps->bmiHeader.biCompression = BI_RGB;
	bitmaps->bmiHeader.biSizeImage = 0;
	bitmaps->bmiHeader.biXPelsPerMeter = 0;
	bitmaps->bmiHeader.biYPelsPerMeter = 0;
	bitmaps->bmiHeader.biClrUsed = 0;
	bitmaps->bmiHeader.biClrImportant = 0;

	if (_bpp == 8)
	{
		for (int i = 0; i < 256; ++i)
		{
			bitmaps->bmiColors[i].rgbBlue = (BYTE)i;
			bitmaps->bmiColors[i].rgbGreen = (BYTE)i;
			bitmaps->bmiColors[i].rgbRed = (BYTE)i;
			bitmaps->bmiColors[i].rgbReserved = 0;
		}
	}

	bitmaps->bmiHeader.biWidth = _w;
	bitmaps->bmiHeader.biHeight = -_h;
}

void CTP1SingleViewDlg::DrawImage()
{
	CreateBitmapInfo(captureImage[isUndistortion]->cols, captureImage[isUndistortion]->rows, captureImage[isUndistortion]->channels() * 8);
	
	CClientDC dc(GetDlgItem(IDC_PICTURE0));

	CRect rect;
	GetDlgItem(IDC_PICTURE0)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0,
		captureImage[isUndistortion]->cols, captureImage[isUndistortion]->rows,
		captureImage[isUndistortion]->data, bitmaps, DIB_RGB_COLORS, SRCCOPY);
}

void CTP1SingleViewDlg::DrawImageBitblt()
{
	CClientDC dc(GetDlgItem(IDC_PICTURE0));

	CRect rect; 
	GetDlgItem(IDC_PICTURE0)->GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	
	cv::resize(*captureImage[isUndistortion], *smallImage, cv::Size(960, 540));

	for (int col = 0; col < 540; ++col)
	{
		for (int row = 0; row < 960; ++row)
		{
			bmpBufferes[col * 960 * 4 + row * 4] = smallImage->data[col * 960 * 3 + row * 3];
			bmpBufferes[col * 960 * 4 + row * 4 + 1] = smallImage->data[col * 960 * 3 + row * 3 + 1];
			bmpBufferes[col * 960 * 4 + row * 4 + 2] = smallImage->data[col * 960 * 3 + row * 3 + 2];
			bmpBufferes[col * 960 * 4 + row * 4 + 3] = 255;
		}
	}

	CBitmap bitmap;
	BOOL re = bitmap.CreateBitmap(960, 540, 1, 32, bmpBufferes);
	memDC.SelectObject(bitmap);

	BOOL re2 = dc.BitBlt(0, 0, 960, 540, &memDC, 0, 0, SRCCOPY);
}

void CTP1SingleViewDlg::InitPicture()
{
	CClientDC dc(GetDlgItem(IDC_PICTURE0));

	CRect rect;
	GetDlgItem(IDC_PICTURE0)->GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	for (int col = 0; col < 540; ++col)
	{
		for (int row = 0; row < 960; ++row)
		{
			bmpBufferes[col * 960 * 4 + row * 4] = 240;
			bmpBufferes[col * 960 * 4 + row * 4 + 1] = 240;
			bmpBufferes[col * 960 * 4 + row * 4 + 2] = 240;
			bmpBufferes[col * 960 * 4 + row * 4 + 3] = 255;
		}
	}

	CBitmap bitmap;
	BOOL re = bitmap.CreateBitmap(960, 540, 1, 32, bmpBufferes);
	memDC.SelectObject(bitmap);

	BOOL re2 = dc.BitBlt(0, 0, 960, 540, &memDC, 0, 0, SRCCOPY);
}

void CTP1SingleViewDlg::InitVideoStreaming()
{
	m_pCaptures = new cv::VideoCapture(((CStringA)streamingAddresses).GetString());
	if (m_pCaptures == nullptr) return;

	captureImage[0] = new cv::Mat();
	if (captureImage[0] == nullptr) return;

	captureImage[1] = new cv::Mat();
	if (captureImage[1] == nullptr) return;

	smallImage = new cv::Mat();
	if (smallImage == nullptr) return;
}

void CTP1SingleViewDlg::Undistort()
{
	int Cx = 960;
	int Cy = 540;
	double Fx = 1804.511;
	double Fy = 1894.737;
	double k1 = -0.093460;
	double k2 = 0.043375;
	double k3 = 0.009760;
	k1 = -0.143460;
	k2 = 0.0;
	k3 = 0.0;

	double Xnu = 0.0;
	double Ynu = 0.0;
	double ru2 = 0.0;
	double radial_d = 0.0;
	double Xnd = 0.0;
	double Ynd = 0.0;
	double Xpd = 0.0;
	double Ypd = 0.0;
	int Xpd2i = 0;
	int Ypd2i = 0;

	*captureImage[1] = cv::Mat::zeros(1080, 1920, CV_8UC3);
	// *undistortImage = distortImage->clone();

	for (int v = 0; v < captureImage[1]->rows; ++v)
	{
		for (int u = 0; u < captureImage[1]->cols; ++u)
		{
			Ynu = (static_cast<double>(v) - static_cast<double>(Cy)) / Fy;
			Xnu = (static_cast<double>(u) - static_cast<double>(Cx)) / Fx;

			ru2 = Xnu * Xnu + Ynu * Ynu;
			radial_d = 1.0 + k1 * ru2 + k2 * ru2 * ru2 + k3 * ru2 * ru2 * ru2;

			Ynd = radial_d * Ynu;
			Xnd = radial_d * Xnu;

			Ypd = Fy * Ynd + Cy;
			Xpd = Fx * Xnd + Cx;

			Ypd2i = static_cast<int>(Ypd);
			Xpd2i = static_cast<int>(Xpd);

			captureImage[1]->data[v * captureImage[1]->cols * 3 + u * 3 + 0] = captureImage[0]->data[Ypd2i * captureImage[0]->cols * 3 + Xpd2i * 3 + 0];
			captureImage[1]->data[v * captureImage[1]->cols * 3 + u * 3 + 1] = captureImage[0]->data[Ypd2i * captureImage[0]->cols * 3 + Xpd2i * 3 + 1];
			captureImage[1]->data[v * captureImage[1]->cols * 3 + u * 3 + 2] = captureImage[0]->data[Ypd2i * captureImage[0]->cols * 3 + Xpd2i * 3 + 2];
		}
	}
}

double CTP1SingleViewDlg::GetRadial(int _u, int _v, double _fx, double _fy)
{
	double Ynu = (static_cast<double>(_v) - 539.5) / _fy;
	double Xnu = (static_cast<double>(_u) - 959.5) / _fx;

	double ru2 = Xnu * Xnu + Ynu * Ynu;
	return 1.0 + (-0.143460 * ru2);
}

void CTP1SingleViewDlg::InferenceStart()
{
	/********** Logging **********/
	CString fileName;
	fileName.Format(_T("%s\\log"), excuteFolderPath.GetString());
	FILE* logFile = nullptr;
	fileOpenError = fopen_s(&logFile, (CStringA)fileName, "a");

	/********** IF_SC100 패킷 초기화 **********/
	unsigned char SendBuf[2048] = { 0, };
	SendBuf[0] = 1; // IF_SC100.Header = 1
	RepetitionData repetitionData;
	memcpy_s(&SendBuf[1], 32, cctvIPs, static_cast<__int64>(cctvIPs.GetLength()) * 2);

	// 비디오 스트리밍 초기화
	InitVideoStreaming();

	/********** 변수 선언 및 초기화 **********/
	// FPS 변수
	std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point nowFrameTime;
	std::chrono::duration<double> timeSpan;
	std::chrono::high_resolution_clock::time_point printLFT = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point printNFT;
	std::chrono::duration<double> printTS;
	CString cStrFPS;
	std::string strFPS;
	int frameCount = 0;

	// Yolo 감지 결과에 사용하는 변수
	std::vector<bbox_t> detectResults;
	std::vector<bbox_t> trackingResults;
	
	int left = 0, right = 0, top = 0, bottom = 0;
	
	// ID 부여에 사용하는 변수
	IDManager idManager;

	// PTZ 상태값 변수
	CString strNowPTZ;
	
	// 위치 추정에 사용하는 변수
	double fx = 1804.511, fy = 1894.737;
	double radial = 0.0;
	double xAxisD = 110.0, yAxisD = 0.0, zAxisD = 0.0;
	double rm[9];
	double lp[3];
	double geo[3];

	// 시간 변수
	CString strNowTime;

	// 로깅에 사용하는 변수
	cv::String classNameTemp;
	CString csLog;

	// 네트워크
	int sendRtn = 0;

	// 화면 랜더링
	bmpBufferes = new unsigned char[2073600];

	while (true)
	{
		if (isYoloThreadRun == false) break;
		if (m_pCaptures == nullptr) break;

		/********** FPS 처리 **********/
		nowFrameTime = std::chrono::high_resolution_clock::now();
		printNFT = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(nowFrameTime - lastFrameTime);
		printTS = std::chrono::duration_cast<std::chrono::duration<double>>(printNFT - printLFT);

		/********** 이미지 캡처 **********/
		// Logging(logFile, _T("이미지 캡처"));
		if (isRtspStreaming == true)
		{
			for (int i = 0; i < 6; ++i)
			{
				if (m_pCaptures->read(*captureImage[0]) == false)
				{
					// Logging(logFile, _T("read false return"));
					m_pCaptures->release();
					m_pCaptures = new cv::VideoCapture(((CStringA)streamingAddresses).GetString());
				}
			}
		}
		else
		{
			if (timeSpan.count() < 0.2) continue;
			for (int i = 0; i < 6; ++i)
			{
				if (m_pCaptures->read(*captureImage[0]) == false)
				{
					m_pCaptures = new cv::VideoCapture(((CStringA)streamingAddresses).GetString());
				}
			}
		}

		if (captureImage[0]->empty() == true) continue;

		// 왜곡 보정
		if (isUndistortion == 1)
			Undistort();

		// Yolo inference
		detectResults = yoloDetector->detect(*captureImage[isUndistortion], 0.35f);

		// ID tracking
		trackingResults = yoloDetector->tracking_id(detectResults, false, 5, trackingMaxDistance);

		/********** CCTV 설치 위치 및 방향에 따른 Offset 적용 **********/
		ApplyOffSet(ptz[0], ptz[1], ptz[2], &xAxisD, &yAxisD, &zAxisD);

		/********** 테이블 비우기 **********/
		listYolo.DeleteAllItems();

		/********** 감지 객체 별 처리 **********/
		for (int i = 0; i < trackingResults.size(); ++i)
		{
			/********** 사각형 좌표 계산 **********/
			left = trackingResults[i].x;
			top = trackingResults[i].y;
			right = trackingResults[i].x + trackingResults[i].w;
			bottom = trackingResults[i].y + trackingResults[i].h;

			/********** 위치 추정 **********/
			GetRorationMatrix(xAxisD, yAxisD, zAxisD, rm);
			LocationEstimation(trackingResults[i].x + trackingResults[i].w / 2, bottom, 1804.511, 1894.737, 960.0, 540.0, rm, xAxisD, lp, geo);

			/********** 테이블에 데이터 표출 **********/
			DisplayDataToTable(i, trackingResults[i], lp[0], lp[1], geo[0], geo[1]);

			/********** 클래스 이름 출력, 사각형 그리기 **********/
			classNameTemp = CT2CA(classNames[trackingResults[i].obj_id]);
			putText(*captureImage[isUndistortion], classNameTemp, cv::Point(left, top), 0, 2, cv::Scalar(255, 178, 50), 3);
			rectangle(*captureImage[isUndistortion], cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(255, 178, 50), 3);

			/********** Logging **********/
			csLog.Format(_T("%s %s %lf %lf\n"), strNowTime.GetString(), classNames[trackingResults[i].obj_id].GetString(), geo[0], geo[1]);
			if (logFile != nullptr) fputsError = fputs((CStringA)csLog, logFile);
		}

		/********** FPS 화면 출력 **********/
		if (printTS.count() > 1.0)
		{
			cStrFPS.Format(_T("%d FPS"), frameCount);
			strFPS = CT2CA(cStrFPS);
			printLFT = printNFT;
			frameCount = 0;
		}
		putText(*captureImage[isUndistortion], strFPS, cv::Point(100, 100), 0, 2, cv::Scalar(255, 0, 0), 3);
				
		/********** 화면에 이미지 그리기 **********/
		DrawImageBitblt();

		/********** FPS **********/
		lastFrameTime = nowFrameTime;
		frameCount++;
		// Logging(logFile, _T("반복문 끝"));
	}

	/********** 영상이 멈추면 화면을 초기화 **********/
	InitPicture();

	/********** 테이블 비우기 **********/
	listYolo.DeleteAllItems();

	/********** CCTV Rtsp 영상 스트리밍 **********/
	m_pCaptures->release();
	m_pCaptures = nullptr;
	captureImage[0]->release();
	captureImage[0] = nullptr;
	captureImage[1]->release();
	captureImage[1] = nullptr;
	smallImage->release();
	smallImage = nullptr;

	/********** Logging **********/
	if (logFile != nullptr)
		fclose(logFile);
}

void CTP1SingleViewDlg::InferenceStop()
{
	/********** 스레드 종료 **********/
	isYoloThreadRun = false;

	isRtspStreaming = false;
}

bool CTP1SingleViewDlg::TryConnectServer()
{
	socketLock.Lock();

	if (m_Socket.isSocketConnected == true)
	{
		socketLock.Unlock();
		return true;
	}

	// 소켓 생성
	m_Socket.Create();

	// SC server 연결
	if (m_Socket.Connect(serverIPStr, 6097) == FALSE)
	{
		m_Socket.Close();
		socketLock.Unlock();
		return false;
	}

	m_Socket.isSocketConnected = true;

	socketLock.Unlock();

	return true;
}

void CTP1SingleViewDlg::RequestHTTP()
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

	while (isRtspStreaming == true)
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
			pConnection = (CHttpConnection*)session.GetHttpConnection(cctvIPs, (INTERNET_PORT)INTERNET_DEFAULT_HTTP_PORT);
			if (pConnection == nullptr) continue;
			pHTTPFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/cgi-bin/param.cgi?action=list&group=PTZPOS&channel=0"));
			if (pHTTPFile == nullptr) continue;
			pHTTPFile->SendRequest(strHeader, 0, 0);
			// if (bResult == 0) continue;
			
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
			ptz[0] = _wtoi(csPan);
			ptz[1] = _wtoi(csTilt);
			ptz[2] = _wtoi(csZoom);
		}
		catch (CInternetException* pEx)
		{
			pConnection = nullptr;
			pEx->Delete();
		}

		lastFrameTime = nowFrameTime;
	}

	delete[] pszBuff;
	if (pHTTPFile != nullptr) pHTTPFile->Close();
	delete pHTTPFile;
	if (pConnection != nullptr) pConnection->Close();
	delete pConnection;
	session.Close();
}

void CTP1SingleViewDlg::ApplyOffSet(int _pan, int _tilt, int _zoom, double* _xAxisD, double* _yAxisD, double* _zAxisD)
{
	*_xAxisD = (18000.0 - static_cast<double>(_tilt)) / 100.0 + offsetY;
	*_yAxisD = static_cast<double>(_pan) / 100.0 + offsetX;
	*_zAxisD = offsetZ;
}

void CTP1SingleViewDlg::GetNowTime(CString* strNowYMDHMSM)
{
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

	strNowYMDHMSM->Format(_T("%d-%02d-%02d %02d:%02d:%02d.%03d%03d"),
		nowSysTime.wYear,
		nowSysTime.wMonth,
		nowSysTime.wDay,
		nowSysTime.wHour,
		nowSysTime.wMinute,
		nowSysTime.wSecond,
		nowSysTime.wMilliseconds,
		static_cast<int>(microSec)
	);
}

void CTP1SingleViewDlg::DisplayDataToTable(int _i, bbox_t _ri, double _lpX, double _lpZ, double _geoLat, double _geoLog)
{
	CString strText = _T("");
	// line Number
	strText.Format(_T("%d"), _i);
	listYolo.InsertItem(_i, strText);

	// T ID
	strText.Format(_T("%d"), _ri.track_id);
	listYolo.SetItemText(_i, 1, strText);

	// Name
	listYolo.SetItemText(_i, 2, classNames[_ri.obj_id]);

	// Left
	strText.Format(_T("%d"), _ri.x);
	listYolo.SetItemText(_i, 3, strText);

	// Top
	strText.Format(_T("%d"), _ri.y);
	listYolo.SetItemText(_i, 4, strText);

	// Right
	strText.Format(_T("%d"), _ri.x + _ri.w);
	listYolo.SetItemText(_i, 5, strText);

	// Bottom
	strText.Format(_T("%d"), _ri.y + _ri.h);
	listYolo.SetItemText(_i, 6, strText);

	// Latitude
	strText.Format(_T("%lf"), _geoLat);
	listYolo.SetItemText(_i, 7, strText);

	// Longitude
	strText.Format(_T("%lf"), _geoLog);
	listYolo.SetItemText(_i, 8, strText);

	// Prob
	strText.Format(_T("%.3f"), _ri.prob);
	listYolo.SetItemText(_i, 9, strText);

	// Local X
	strText.Format(_T("%lf"), _lpX);
	listYolo.SetItemText(_i, 10, strText);

	// Local Z
	strText.Format(_T("%lf"), _lpZ);
	listYolo.SetItemText(_i, 11, strText);
}

void CTP1SingleViewDlg::LocationEstimation(int u, int v, double fx, double fy, double cx, double cy, double* r, double xDegree, double* _lp, double* _geo)
{
	// 선택된 uv 좌표를 카메라 좌표계 벡터로 변환
	double cn[] = { (static_cast<double>(u) - cx) / fx, (static_cast<double>(v) - cy) / fy, 1.0 };

	double t[] = { 0.0, sin(xDegree * CV_PI / 180.0) * cameraHeight, -cos(xDegree * CV_PI / 180.0) * cameraHeight };

	// 카메라 설치 높이(Z) ÷ 카메라 좌표계 벡터 높이(Z)
	double s = (r[2] * t[0] + r[5] * t[1] + r[8] * t[2]) / (r[2] * cn[0] + r[5] * cn[1] + r[8] * cn[2]);

	// 월드 좌표계 Ray 방향 벡터
	double ray[3];
	Mul3_1X3_3(cn, r, ray);

	// 월드 좌표계 카메라 좌표
	double zeroPoint[3];
	Mul3_1X3_3(t, r, zeroPoint);

	// Local coordinate value
	_lp[0] = ray[0] * s - zeroPoint[0];
	_lp[1] = ray[1] * s - zeroPoint[1];
	_lp[2] = ray[2] * s - zeroPoint[2];

	double panRadian = (static_cast<double>(ptz[0]) / 100.0 + offsetX) * CV_PI / 180.0;
	double localARX = cos(panRadian) * _lp[0] + sin(panRadian) * _lp[1];
	double localARZ = -sin(panRadian) * _lp[0] + cos(panRadian) * _lp[1];

	double tmEast = cctvTMEast + localARX;
	double tmNorth = cctvTMNorth + localARZ;

	GEO geo = geoChange.TMtoGeo(tmEast, tmNorth, _lp[2]);

	_geo[0] = geo.lat;
	_geo[1] = geo.log;
	_geo[2] = geo.alt;
}

void CTP1SingleViewDlg::GetRorationMatrix(double x, double y, double z, double* rm)
{
	double thetaX = x * CV_PI / 180.0;
	double thetaY = y * CV_PI / 180.0;
	double thetaZ = z * CV_PI / 180.0;

	double AxisXR[] = { 1.0, 0.0, 0.0,
						0.0, cos(thetaX), -sin(thetaX),
						0.0, sin(thetaX), cos(thetaX) };

	double AxisYR[] = { cos(thetaY), 0.0, -sin(thetaY),
						0.0, 1.0, 0.0,
						sin(thetaY), 0.0, cos(thetaY) };

	double AxisZR[] = { cos(thetaZ), -sin(thetaZ), 0.0,
						sin(thetaZ), cos(thetaZ), 0.0,
						0.0, 0.0, 1.0 };

	double AxisXYR[9];
	Mul3_3X3_3(AxisXR, AxisYR, AxisXYR);
	Mul3_3X3_3(AxisXYR, AxisZR, rm);
}

void CTP1SingleViewDlg::Mul3_3X3_3(double* f, double* b, double* result)
{
	result[0] = f[0] * b[0] + f[1] * b[3] + f[2] * b[6];
	result[1] = f[0] * b[1] + f[1] * b[4] + f[2] * b[7];
	result[2] = f[0] * b[2] + f[1] * b[5] + f[2] * b[8];
	result[3] = f[3] * b[0] + f[4] * b[3] + f[5] * b[6];
	result[4] = f[3] * b[1] + f[4] * b[4] + f[5] * b[7];
	result[5] = f[3] * b[2] + f[4] * b[5] + f[5] * b[8];
	result[6] = f[6] * b[0] + f[7] * b[3] + f[8] * b[6];
	result[7] = f[6] * b[1] + f[7] * b[4] + f[8] * b[7];
	result[8] = f[6] * b[2] + f[7] * b[5] + f[8] * b[8];
}

void CTP1SingleViewDlg::Mul3_1X3_3(double* f, double* b, double* result)
{
	result[0] = f[0] * b[0] + f[1] * b[3] + f[2] * b[6];
	result[1] = f[0] * b[1] + f[1] * b[4] + f[2] * b[7];
	result[2] = f[0] * b[2] + f[1] * b[5] + f[2] * b[8];
}

void CTP1SingleViewDlg::Logging(FILE* _file, CString _csMemo)
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
	if (_file != nullptr) fputsError = fputs((CStringA)csLog, _file);
}