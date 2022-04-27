// TP1SingleViewDlg.h: 헤더 파일
#pragma once

#include "opencv2/opencv.hpp"
#include "yolo_v2_class.hpp"
#include "CClientSocket.h"
#include "GeoChange.h"

#pragma pack(push, 1)
struct RepetitionData
{
	unsigned char objType; // 1 | 0 사람 1 차량 2 미확인
	float XAxis;  // 4
	float ZAxis;  // 4
	int objX1; // 4
	int objY1; // 4
	int objX2; // 4
	int objY2; // 4
	float percent; // 4 | 0 ~ 100
};
#pragma pack(pop)

// CTP1SingleViewDlg 대화 상자
class CTP1SingleViewDlg : public CDialogEx
{
// 생성입니다.
public:
	CTP1SingleViewDlg(CWnd* pParent = nullptr); // 표준 생성자입니다

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TP1SingleView_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString excuteFolderPath; // 실행 프로그램 폴더 경로

	std::vector<CString> classNames; // 클래스 벡터

	// Thread
	std::thread* yoloThreads;
	std::atomic<bool> isYoloThreadRun;

	// Streaming video
	CString streamingAddresses;
	std::thread* videoStreamingThreads;
	cv::VideoCapture* m_pCaptures;
	cv::Mat* captureImage[2]; // 1920 X 1080 | 0 : Distortion image, 1 : Undistortion image
	cv::Mat* smallImage; // 960 X 540
	BITMAPINFO* bitmaps; // Bitmap 정보를 담고 있는 구조체
	unsigned char* bmpBufferes;
	std::atomic<bool> isRtspStreaming;

	// Yolo State
	bool isInferencing;

	// Undistortion
	int isUndistortion;

	// ID tracking
	int trackingMaxDistance;

	// PTZ
	CString cctvIPs;
	std::thread* getPTZThreads;
	std::atomic<int> ptz[3];

	// Location etimation
	GeoChange geoChange;
	double latitude;
	double longitude;
	double cctvTMNorth;
	double cctvTMEast;
	double offsetX;
	double offsetY;
	double offsetZ;
	double cameraHeight;

	// UI
	CIPAddressCtrl serverIP;
	CButton networkStartStopBtn;
	bool isNetBtnPushed;
	CButton yoloStartStopBtn;
	bool isButtonPushed;
	CListCtrl listYolo;
	
	// Network interface
	CClientSocket m_Socket;
	CString serverIPStr;
	CCriticalSection socketLock;
	bool communicationStatus;

	// Debug
	int fileOpenError;
	int fputsError;

	// Defalut
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual BOOL DestroyWindow();

	// UI
	afx_msg void OnBtnClickedNetworkbutton();
	afx_msg void OnBtnClicked();
	void CreateBitmapInfo(int _w, int _h, int _bpp); // Bitmap 정보를 생성하는 함수
	void DrawImage(); // 그리는 작업을 수행하는 함수
	void DrawImageBitblt(); // 그리는 작업을 수행하는 함수
	void InitPicture();

	// 영상 스트리밍
	void InitVideoStreaming();

	// 왜곡 보정
	void Undistort();
	double GetRadial(int _u, int _v, double _fx, double _fy);

	// Inference
	void InferenceStart();
	void InferenceStop();

	// 네트워크
	bool TryConnectServer();

	// CCTV PTZ 값
	void RequestHTTP();
	void ApplyOffSet(int, int, int, double*, double*, double*);

	// 시간
	void GetNowTime(CString* strNowYMDHMSM);

	// 테이블
	void DisplayDataToTable(int _i, bbox_t _ri, double _lpX, double _lpZ, double _geoLat, double _geoLog);

	// 위치 추정
	void LocationEstimation(int u, int v, double fx, double fy, double cx, double cy, double* r, double xDegree, double* wp, double* _geo);
	void Mul3_3X3_3(double*, double*, double*);
	void Mul3_1X3_3(double*, double*, double*);
	void GetRorationMatrix(double x, double y, double z, double* rm);

	// 로깅
	void Logging(FILE* _file, CString _csMemo);
};
