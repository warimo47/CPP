// TP1Dlg.cpp: 구현 파일

#include <cmath>
#include <chrono>

#include "pch.h"
#include "framework.h"
#include "TP1.h"
#include "TP1Dlg.h"
#include "afxdialogex.h"
#include "Resource.h"

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

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CTP1Dlg 대화 상자
CTP1Dlg::CTP1Dlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_TP1_DIALOG, pParent)
{
	for (int i = 0; i < 4; ++i)
	{
		// Thread
		yoloThreads[i] = nullptr;
		isYoloThreadRun[i] = false;

		// Streaming video
		videoStreamingThreads[i] = nullptr;
		m_pCaptures[i] = nullptr;
		m_matImages[i] = nullptr;
		m_matImageTs[i] = nullptr;
		bitmaps[i] = nullptr;
		isRtspStreaming[i] = false;

		// Yolo
		isInferencing[i] = false;

		// UI
		isButtonPushed[i] = false;
	}

	// Yolo
	yoloDetector = nullptr;

	// UI
	isNetBtnPushed = false;

	// Network communication
	cm = nullptr;

	// LocationEstimation
	le = nullptr;

	// Debug
	fileOpenError = 0;
	fputsError = 0;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTP1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, serverIP);
	DDX_Control(pDX, IDC_LIST_YOLO0, listYolo[0]);
	DDX_Control(pDX, IDC_LIST_YOLO1, listYolo[1]);
	DDX_Control(pDX, IDC_LIST_YOLO2, listYolo[2]);
	DDX_Control(pDX, IDC_LIST_YOLO3, listYolo[3]);
	DDX_Control(pDX, IDC_BUTTON0, yoloStartStopBtn[0]);
	DDX_Control(pDX, IDC_BUTTON1, yoloStartStopBtn[1]);
	DDX_Control(pDX, IDC_BUTTON2, yoloStartStopBtn[2]);
	DDX_Control(pDX, IDC_BUTTON3, yoloStartStopBtn[3]);
	DDX_Control(pDX, IDC_NETWORKBUTTON, networkStartStopBtn);
}

BEGIN_MESSAGE_MAP(CTP1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND_RANGE(IDC_BUTTON0, IDC_BUTTON3, CTP1Dlg::OnBtnClicked)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_NETWORKBUTTON, &CTP1Dlg::OnBtnClickedNetworkbutton)
END_MESSAGE_MAP()

// CTP1Dlg 메시지 처리기
BOOL CTP1Dlg::OnInitDialog()
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
	cm = new CommunicationManager();
	le = new LocationEstimation();
	le->pptz = cm->ptz;

	/********** INI 파일 정보 가져오기 **********/
	// ini 파일 열기 / 파일 읽을 때, 쓰는 변수 초기화
	wchar_t excuteProgramPath[256] = { 0, };
	GetModuleFileName(NULL, excuteProgramPath, sizeof(excuteProgramPath) / 2);
	excuteFolderPath = excuteProgramPath;
	excuteFolderPath = excuteFolderPath.Left(excuteFolderPath.ReverseFind(_T('\\')));
	CString iniFilePath = excuteFolderPath + "\\init.ini";
	int wcaRS = 1024; // wcaReturnSize
	wchar_t* wcaReturn = new wchar_t[wcaRS];

	// Read CCTV Streaming path app name
	GetPrivateProfileString(_T("CCTV Streaming path"), _T("leftTop"), _T(""), wcaReturn, wcaRS, iniFilePath);
	streamingAddresses[0] = wcaReturn;
	GetPrivateProfileString(_T("CCTV Streaming path"), _T("rightTop"), _T(""), wcaReturn, wcaRS, iniFilePath);
	streamingAddresses[1] = wcaReturn;
	GetPrivateProfileString(_T("CCTV Streaming path"), _T("leftBottom"), _T(""), wcaReturn, wcaRS, iniFilePath);
	streamingAddresses[2] = wcaReturn;
	GetPrivateProfileString(_T("CCTV Streaming path"), _T("rightBottom"), _T(""), wcaReturn, wcaRS, iniFilePath);
	streamingAddresses[3] = wcaReturn;

	int curPos = 0;
	if (streamingAddresses[0].Left(4) != _T("rtsp"))
	{
		isRtspStreaming[0] = false;
		GetPrivateProfileString(_T("CCTV Streaming path"), _T("leftTopIP"), _T(""), wcaReturn, wcaRS, iniFilePath);
		cm->cctvIPs[0] = wcaReturn;
	}
	else
	{
		isRtspStreaming[0] = true;
		curPos = 0;
		streamingAddresses[0].Tokenize(_T("@"), curPos);
		cm->cctvIPs[0] = streamingAddresses[0].Tokenize(_T(":"), curPos);
	}

	if (streamingAddresses[1].Left(4) != _T("rtsp"))
	{
		isRtspStreaming[1] = false;
		GetPrivateProfileString(_T("CCTV Streaming path"), _T("rightTopIP"), _T(""), wcaReturn, wcaRS, iniFilePath);
		cm->cctvIPs[1] = wcaReturn;
	}
	else
	{
		isRtspStreaming[1] = true;
		curPos = 0;
		streamingAddresses[1].Tokenize(_T("@"), curPos);
		cm->cctvIPs[1] = streamingAddresses[1].Tokenize(_T(":"), curPos);
	}

	if (streamingAddresses[2].Left(4) != _T("rtsp"))
	{
		isRtspStreaming[2] = false;
		GetPrivateProfileString(_T("CCTV Streaming path"), _T("rightTopIP"), _T(""), wcaReturn, wcaRS, iniFilePath);
		cm->cctvIPs[2] = wcaReturn;
	}
	else
	{
		isRtspStreaming[2] = true;
		curPos = 0;
		streamingAddresses[2].Tokenize(_T("@"), curPos);
		cm->cctvIPs[2] = streamingAddresses[2].Tokenize(_T(":"), curPos);
	}

	if (streamingAddresses[3].Left(4) != _T("rtsp"))
	{
		isRtspStreaming[3] = false;
		GetPrivateProfileString(_T("CCTV Streaming path"), _T("rightTopIP"), _T(""), wcaReturn, wcaRS, iniFilePath);
		cm->cctvIPs[3] = wcaReturn;
	}
	else
	{
		isRtspStreaming[3] = true;
		curPos = 0;
		streamingAddresses[3].Tokenize(_T("@"), curPos);
		cm->cctvIPs[3] = streamingAddresses[3].Tokenize(_T(":"), curPos);
	}

	// Read CCTV Index app name
	CString csCCTVIndex;
	GetPrivateProfileString(_T("CCTV Index"), _T("leftTop"), _T(""), wcaReturn, wcaRS, iniFilePath);
	csCCTVIndex = wcaReturn;
	cm->cctvIndex[0] = _ttoi(csCCTVIndex);
	GetPrivateProfileString(_T("CCTV Index"), _T("rightTop"), _T(""), wcaReturn, wcaRS, iniFilePath);
	csCCTVIndex = wcaReturn;
	cm->cctvIndex[1] = _ttoi(csCCTVIndex);
	GetPrivateProfileString(_T("CCTV Index"), _T("leftBottom"), _T(""), wcaReturn, wcaRS, iniFilePath);
	csCCTVIndex = wcaReturn;
	cm->cctvIndex[2] = _ttoi(csCCTVIndex);
	GetPrivateProfileString(_T("CCTV Index"), _T("rightBottom"), _T(""), wcaReturn, wcaRS, iniFilePath);
	csCCTVIndex = wcaReturn;
	cm->cctvIndex[3] = _ttoi(csCCTVIndex);

	// Read Yolo app name
	std::wstring wsReturn;
	std::string cfgFileName;
	GetPrivateProfileString(_T("Yolo"), _T("cfg"), _T(""), wcaReturn, wcaRS, iniFilePath);
	wsReturn = wcaReturn;
	cfgFileName.assign(wsReturn.begin(), wsReturn.end());
	std::string weightsFileName;
	GetPrivateProfileString(_T("Yolo"), _T("weights"), _T(""), wcaReturn, wcaRS, iniFilePath);
	wsReturn = wcaReturn;
	weightsFileName.assign(wsReturn.begin(), wsReturn.end());
	std::wstring namesFileName;
	GetPrivateProfileString(_T("Yolo"), _T("names"), _T(""), wcaReturn, wcaRS, iniFilePath);
	namesFileName = wcaReturn;

	// Read LocationEstimation app name
	wchar_t* stopwcs = nullptr;
	GetPrivateProfileString(_T("LocationEstimation"), _T("latitude0"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->latitude[0] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("longitude0"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->longitude[0] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("cameraHeight0"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->cameraHeight[0] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetX0"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetX[0] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetY0"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetY[0] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetZ0"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetZ[0] = wcstod(wcaReturn, &stopwcs);
	
	TM cctvLocationTM = le->geoChange.GeotoTM(le->latitude[0], le->longitude[0], le->cameraHeight[0]);
	le->cctvTMEast[0] = cctvLocationTM.X;
	le->cctvTMNorth[0] = cctvLocationTM.Y;

	GetPrivateProfileString(_T("LocationEstimation"), _T("latitude1"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->latitude[1] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("longitude1"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->longitude[1] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("cameraHeight1"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->cameraHeight[1] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetX1"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetX[1] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetY1"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetY[1] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetZ1"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetZ[1] = wcstod(wcaReturn, &stopwcs);
	
	cctvLocationTM = le->geoChange.GeotoTM(le->latitude[1], le->longitude[1], le->cameraHeight[1]);
	le->cctvTMEast[1] = cctvLocationTM.X;
	le->cctvTMNorth[1] = cctvLocationTM.Y;

	GetPrivateProfileString(_T("LocationEstimation"), _T("latitude2"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->latitude[2] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("longitude2"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->longitude[2] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("cameraHeight2"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->cameraHeight[2] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetX2"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetX[2] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetY2"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetY[2] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetZ2"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetZ[2] = wcstod(wcaReturn, &stopwcs);
	
	cctvLocationTM = le->geoChange.GeotoTM(le->latitude[2], le->longitude[2], le->cameraHeight[2]);
	le->cctvTMEast[2] = cctvLocationTM.X;
	le->cctvTMNorth[2] = cctvLocationTM.Y;

	GetPrivateProfileString(_T("LocationEstimation"), _T("latitude3"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->latitude[3] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("longitude3"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->longitude[3] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("cameraHeight3"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->cameraHeight[3] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetX3"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetX[3] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetY3"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetY[3] = wcstod(wcaReturn, &stopwcs);
	GetPrivateProfileString(_T("LocationEstimation"), _T("offsetZ3"), _T(""), wcaReturn, wcaRS, iniFilePath);
	le->offsetZ[3] = wcstod(wcaReturn, &stopwcs);
	
	cctvLocationTM = le->geoChange.GeotoTM(le->latitude[3], le->longitude[3], le->cameraHeight[3]);
	le->cctvTMEast[3] = cctvLocationTM.X;
	le->cctvTMNorth[3] = cctvLocationTM.Y;

	// Network app name 읽기
	CString csDefaultIP, csTemp;
	int octet0, octet1, octet2, octet3 = 0;
	GetPrivateProfileString(_T("Network"), _T("DefaultIP"), _T(""), wcaReturn, wcaRS, iniFilePath);
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
	serverIP.SetAddress(octet0, octet1, octet2, octet3);

	CString csCommunicationStatus;
	GetPrivateProfileString(_T("Network"), _T("CommunicationStatus"), _T(""), wcaReturn, wcaRS, iniFilePath);
	csCommunicationStatus = wcaReturn;
	if (csCommunicationStatus == _T("True") || csCommunicationStatus == _T("true"))
		cm->communicationStatus = true;
	if (cm->communicationStatus == false)
	{
		serverIP.EnableWindow(0); // Server IP 입력 창 비활성화
		networkStartStopBtn.EnableWindow(0); // 서버 연결 버튼 비활성화
		for (int thc = 0/* Thread count */; thc < 4; ++thc)
			yoloStartStopBtn[thc].EnableWindow();
	}

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
	for (int thc = 0/* Thread count */; thc < 4; ++thc)
	{
		listYolo[thc].InsertColumn(0, _T("No"), LVCFMT_CENTER, 40);
		listYolo[thc].InsertColumn(1, _T("T ID"), LVCFMT_CENTER, 40);
		listYolo[thc].InsertColumn(2, _T("Object Name"), LVCFMT_LEFT, 130);
		listYolo[thc].InsertColumn(3, _T("Left"), LVCFMT_LEFT, 60);
		listYolo[thc].InsertColumn(4, _T("Top"), LVCFMT_LEFT, 60);
		listYolo[thc].InsertColumn(5, _T("Right"), LVCFMT_LEFT, 60);
		listYolo[thc].InsertColumn(6, _T("Bottom"), LVCFMT_LEFT, 60);
		listYolo[thc].InsertColumn(7, _T("Latitude"), LVCFMT_LEFT, 80);
		listYolo[thc].InsertColumn(8, _T("Longitude"), LVCFMT_LEFT, 80);
		listYolo[thc].InsertColumn(9, _T("Prob"), LVCFMT_LEFT, 100);
		listYolo[thc].InsertColumn(10, _T("Local X"), LVCFMT_LEFT, 80);
		listYolo[thc].InsertColumn(11, _T("Local Z"), LVCFMT_LEFT, 80);
		listYolo[thc].SetExtendedStyle(LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	}

	delete[] wcaReturn;

	return TRUE; // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTP1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
void CTP1Dlg::OnPaint()
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
HCURSOR CTP1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTP1Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	delete yoloDetector;
	yoloDetector = nullptr;
}

void CTP1Dlg::OnClose()
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

BOOL CTP1Dlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::DestroyWindow();
}

void CTP1Dlg::OnBtnClickedNetworkbutton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (isNetBtnPushed == false)
	{
		// Get server IP from UI
		serverIP.GetWindowText(cm->serverIPStr);

		// Try connect server
		if (cm->TryConnectServer() == false) return;

		// Prevent IP address changes while connected
		serverIP.EnableWindow(0);

		// Each video can be started
		for (int i = 0; i < 4; ++i)
			yoloStartStopBtn[i].EnableWindow();
		
		// Change button UI letter
		networkStartStopBtn.SetWindowTextW(_T("SC 연결 종료"));

		isNetBtnPushed = true;
	}
	else
	{
		// Terminate communication connection
		cm->SocketClose();
		
		// Enable IP address change
		serverIP.EnableWindow();

		// Don't let all the videos start
		for (int i = 0; i < 4; ++i)
			yoloStartStopBtn[i].EnableWindow(0);

		// Change button UI letter
		networkStartStopBtn.SetWindowTextW(_T("SC 연결 시작"));
	
		isNetBtnPushed = false;
	}
}

void CTP1Dlg::OnBtnClicked(unsigned int _ctrlID)
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

		// 하나라도 실행 중이면 네트워크 연결 종료 불가
		networkStartStopBtn.EnableWindow(0);

		// 버튼 문자열
		btnStr.Format(_T("%d번 영상 Yolo Stop"), threadNum);

		// UI
		yoloStartStopBtn[threadNum].SetWindowTextW(btnStr);

		// 객체 감지 및 위치 추정 시작
		isYoloThreadRun[threadNum] = true;
		yoloThreads[threadNum] = new std::thread(&CTP1Dlg::InferenceStart, this, threadNum);

		// PTZ 상태 값 수신 시작
		// if (isRtspStreaming[threadNum] == true)
			// cm.Try;
	}
	else
	{
		// 버튼 상태
		isButtonPushed[threadNum] = false;

		// PTZ 값을 가져오고 있었다면 중지
		if (isRtspStreaming[threadNum] == true)
			isRtspStreaming[threadNum] = false;
		
		// SC와 통신하는 설정 인 경우
		if (cm->communicationStatus == true)
		{
			// 모든 영상이 종료되었으면 네트워크 연결 종료 가능
			bool totalFinish = false;
			for (int i = 0; i < 4; ++i)
			{
				if (isButtonPushed[i] == true) totalFinish = true;
			}
			if (totalFinish == false) networkStartStopBtn.EnableWindow();
		}

		// 버튼 문자열
		btnStr.Format(_T("%d번 영상 Yolo Start"), threadNum);

		// UI
		yoloStartStopBtn[threadNum].SetWindowTextW(btnStr);

		// 객체 감지 및 위치 추정 종료
		InferenceStop(threadNum);
	}
}

void CTP1Dlg::CreateBitmapInfo(int _threadNum, int w, int h, int bpp)
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

void CTP1Dlg::DrawImage(int _threadNum)
{
	CreateBitmapInfo(_threadNum, m_matImages[_threadNum]->cols, m_matImages[_threadNum]->rows, m_matImages[_threadNum]->channels() * 8);
	
	CClientDC dc(GetDlgItem(IDC_PICTURE0 + _threadNum));

	CRect rect;
	GetDlgItem(IDC_PICTURE0 + _threadNum)->GetClientRect(&rect);

	SetStretchBltMode(dc.GetSafeHdc(), HALFTONE);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_matImages[_threadNum]->cols, m_matImages[_threadNum]->rows, m_matImages[_threadNum]->data, bitmaps[_threadNum], DIB_RGB_COLORS, SRCCOPY);
}

void CTP1Dlg::DrawImageBitblt(int _threadNum)
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

void CTP1Dlg::InitVideoStreaming(int _threadNum)
{
	m_pCaptures[_threadNum] = new cv::VideoCapture(((CStringA)streamingAddresses[_threadNum]).GetString());
	if (m_pCaptures[_threadNum] == nullptr) return;

	m_matImages[_threadNum] = new cv::Mat();
	if (m_matImages[_threadNum] == nullptr) return;

	m_matImageTs[_threadNum] = new cv::Mat();
	if (m_matImageTs[_threadNum] == nullptr) return;
}

void CTP1Dlg::InferenceStart(int _thn /* Thread Num */)
{
	/********** Logging **********/
	CString fileName;
	fileName.Format(_T("%s\\log%d"), excuteFolderPath.GetString(), _thn);
	FILE* logFile = nullptr;
	fileOpenError = fopen_s(&logFile, (CStringA)fileName, "a");

	// 비디오 스트리밍 초기화
	InitVideoStreaming(_thn);

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
	
	// 위치 추정에 사용하는 변수
	double xAxisD = 110.0, yAxisD = 0.0, zAxisD = 0.0;
	double rm[9];
	double lp[3];
	double geo[3];

	// 로깅에 사용하는 변수
	cv::String classNameTemp;
	CString csLog;

	// 화면 랜더링
	bmpBufferes[_thn] = new unsigned char[2073600];

	while (true)
	{
		if (isYoloThreadRun[_thn] == false) break;
		if (m_pCaptures[_thn] == nullptr) break;

		/********** FPS 처리 **********/
		nowFrameTime = std::chrono::high_resolution_clock::now();
		printNFT = std::chrono::high_resolution_clock::now();
		timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(nowFrameTime - lastFrameTime);
		printTS = std::chrono::duration_cast<std::chrono::duration<double>>(printNFT - printLFT);

		/********** 이미지 캡처 **********/
		// Logging(logFile, _T("이미지 캡처"));
		if (isRtspStreaming[_thn] == true)
		{
			for (int i = 0; i < 6; ++i)
			{
				if (m_pCaptures[_thn]->read(*m_matImages[_thn]) == false)
				{
					// Logging(logFile, _T("read false return"));
					m_pCaptures[_thn]->release();
					m_pCaptures[_thn] = new cv::VideoCapture(((CStringA)streamingAddresses[_thn]).GetString());
				}
			}
		}
		else
		{
			if (timeSpan.count() < 0.2) continue;
			for (int i = 0; i < 6; ++i)
			{
				if (m_pCaptures[_thn]->read(*m_matImages[_thn]) == false)
				{
					m_pCaptures[_thn] = new cv::VideoCapture(((CStringA)streamingAddresses[_thn]).GetString());
				}
			}
		}

		/********** Yolo inference 수행 **********/
		if (m_matImages[_thn]->empty() == true) continue;
		// Logging(logFile, _T("Inference"));
		detectorLock.Lock();
		detectResults = yoloDetector->detect(*m_matImages[_thn], 0.35f);
		trackingResults = yoloDetector->tracking_id(detectResults, true, 5, 100);
		detectorLock.Unlock();
				
		/********** CCTV 설치 위치 및 방향에 따른 Offset 적용 **********/
		if (isRtspStreaming[_thn] == true)
		{
			le->ApplyOffSet(_thn, &xAxisD, &yAxisD, &zAxisD);
		}

		/********** 테이블 비우기 **********/
		listYolo[_thn].DeleteAllItems();

		/********** IF_SC100 패킷 헤더 데이터 입력 **********/
		// Logging(logFile, _T("패킷 헤더 데이터 입력"));
		cm->SetTime(_thn);

		cm->resultSize[_thn] = 0;
		
		/********** 감지 객체 별 처리 **********/
		// Logging(logFile, _T("감지 객체 별 처리"));
		for (int i = 0; i < trackingResults.size(); ++i)
		{
			// if (results[i].obj_id != 0) continue;

			/********** 사각형 좌표 계산 **********/
			left = trackingResults[i].x;
			top = trackingResults[i].y;
			right = trackingResults[i].x + trackingResults[i].w;
			bottom = trackingResults[i].y + trackingResults[i].h;

			/********** 위치 추정 **********/
			le->GetRorationMatrix(xAxisD, yAxisD, zAxisD, rm);
			le->GetWorldPosition(_thn, trackingResults[i].x + trackingResults[i].w / 2, bottom, 1804.511, 1894.737, 960.0, 540.0, rm, xAxisD, lp, geo);

			/********** 통신 인터페이스 데이터 입력 **********/
			cm->SetRepetitionData(_thn, trackingResults[i].track_id, trackingResults[i].obj_id, (float)lp[0], (float)lp[1], left, right, top, bottom, trackingResults[i].prob * 100.f);

			/********** 테이블에 데이터 표출 **********/
			DisplayDataToTable(_thn, i, trackingResults[i], lp[0], lp[1], geo[0], geo[1]);

			/********** 클래스 이름 출력, 사각형 그리기 **********/
			classNameTemp = CT2CA(classNames[trackingResults[i].obj_id]);
			putText(*m_matImages[_thn], classNameTemp, cv::Point(left, top), 0, 2, cv::Scalar(255, 178, 50), 3);
			rectangle(*m_matImages[_thn], cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(255, 178, 50), 3);

			/********** Logging **********/
			// csLog.Format(_T("%s %s %lf %lf\n"), strNowTime.GetString(), classNames[results[i].obj_id].GetString(), wp[0], wp[1]);
			// if (logFile != nullptr) fputsError = fputs((CStringA)csLog, logFile);
		}

		cm->SetSize(_thn);
		cm->SendPacket(_thn);

		/********** FPS 화면 출력 **********/
		if (printTS.count() > 1.0)
		{
			cStrFPS.Format(_T("%d FPS"), frameCount);
			strFPS = CT2CA(cStrFPS);
			printLFT = printNFT;
			frameCount = 0;
		}
		putText(*m_matImages[_thn], strFPS, cv::Point(100, 100), 0, 2, cv::Scalar(255, 0, 0), 3);
		
		/********** 화면에 이미지 그리기 **********/
		// Logging(logFile, _T("화면 그리기"));
		DrawImageBitblt(_thn);
		// DrawImage(_threadNum);
		
		/********** FPS **********/
		lastFrameTime = nowFrameTime;
		frameCount++;
		// Logging(logFile, _T("반복문 끝"));
	}

	/********** 영상이 멈추면 화면을 초기화 **********/
	if (m_matImages[_thn] != nullptr)
	{
		for (int v = 0; v < m_matImages[_thn]->rows; ++v)
		{
			for (int u = 0; u < m_matImages[_thn]->cols; ++u)
			{
				m_matImages[_thn]->data[v * m_matImages[_thn]->cols * 3 + u * 3 + 0] = 240;
				m_matImages[_thn]->data[v * m_matImages[_thn]->cols * 3 + u * 3 + 1] = 240;
				m_matImages[_thn]->data[v * m_matImages[_thn]->cols * 3 + u * 3 + 2] = 240;
			}
		}
	}
	DrawImage(_thn);

	/********** 테이블 비우기 **********/
	listYolo[_thn].DeleteAllItems();

	/********** CCTV Rtsp 영상 스트리밍 **********/
	m_pCaptures[_thn]->release();
	m_pCaptures[_thn] = nullptr;
	m_matImages[_thn]->release();
	m_matImages[_thn] = nullptr;
	m_matImageTs[_thn]->release();
	m_matImageTs[_thn] = nullptr;

	/********** Logging **********/
	if (logFile != nullptr)
		fclose(logFile);
}

void CTP1Dlg::InferenceStop(int _thn)
{
	/********** 스레드 종료 **********/
	isYoloThreadRun[_thn] = false;

	isRtspStreaming[_thn] = false;
}

void CTP1Dlg::DisplayDataToTable(int _threadNum, int _i, bbox_t _ri, double _lpX, double _lpZ, double _geoLat, double _geoLog)
{
	CString strText = _T("");
	// line Number
	strText.Format(_T("%d"), _i);
	listYolo[_threadNum].InsertItem(_i, strText);

	// Tracking ID
	strText.Format(_T("%d"), _ri.track_id);
	listYolo[_threadNum].SetItemText(_i, 1, strText);

	// Name
	listYolo[_threadNum].SetItemText(_i, 2, classNames[_ri.obj_id]);

	// Left
	strText.Format(_T("%d"), _ri.x);
	listYolo[_threadNum].SetItemText(_i, 3, strText);

	// Top
	strText.Format(_T("%d"), _ri.y);
	listYolo[_threadNum].SetItemText(_i, 4, strText);

	// Right
	strText.Format(_T("%d"), _ri.x + _ri.w);
	listYolo[_threadNum].SetItemText(_i, 5, strText);

	// Bottom
	strText.Format(_T("%d"), _ri.y + _ri.h);
	listYolo[_threadNum].SetItemText(_i, 6, strText);

	// Latitude
	strText.Format(_T("%lf"), _geoLat);
	listYolo[_threadNum].SetItemText(_i, 7, strText);

	// Longitude
	strText.Format(_T("%lf"), _geoLog);
	listYolo[_threadNum].SetItemText(_i, 8, strText);

	// Prob
	strText.Format(_T("%.3f"), _ri.prob);
	listYolo[_threadNum].SetItemText(_i, 9, strText);

	// Local X
	strText.Format(_T("%lf"), _lpX);
	listYolo[_threadNum].SetItemText(_i, 10, strText);

	// Local Z
	strText.Format(_T("%lf"), _lpZ);
	listYolo[_threadNum].SetItemText(_i, 11, strText);
}

void CTP1Dlg::Logging(FILE* _file, CString _csMemo)
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