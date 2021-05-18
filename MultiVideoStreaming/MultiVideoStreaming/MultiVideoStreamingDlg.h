// MultiVideoStreamingDlg.h: 헤더 파일
#pragma once

#include "opencv2/opencv.hpp"
#include "yolo_v2_class.hpp"

// CMultiVideoStreamingDlg 대화 상자
class CMultiVideoStreamingDlg : public CDialogEx
{
// 생성입니다.
public:
	CMultiVideoStreamingDlg(CWnd* pParent = nullptr); // 표준 생성자입니다

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MultiVideoStreaming_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV 지원입니다

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString excuteFolderPath; // 실행 프로그램 폴더 경로

	// 영상 스트리밍
	CString streamingAddresses[4];
	std::thread* videoStreamingThreads[4];
	cv::VideoCapture* m_pCaptures[4];
	cv::Mat* m_matImages[4]; // 이미지 정보를 담고 있는 객체
	cv::Mat* m_matImageTs[4]; // 이미지 복사용 객체
	BITMAPINFO* bitmaps[4]; // Bitmap 정보를 담고 있는 구조체
	unsigned char* bmpBufferes[4];
	std::atomic<bool> isVideoStreaming[4];

	// UI
	CButton videoStreamingStartStopBtn[4];
	bool isButtonPushed[4];
	
	// Defalut
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual BOOL DestroyWindow();

	// UI
	afx_msg void OnBtnClicked(unsigned int _ctrlID);
	void CreateBitmapInfo(int _threadNum, int w, int h, int bpp); // Bitmap 정보를 생성하는 함수
	void DrawImage(int); // 그리는 작업을 수행하는 함수
	void DrawImageBitblt(int);

	// 영상 스트리밍
	void InitVideoStreaming(int _threadNum);
	void VideoStreaming(int _threadNum);

	// 디버깅
	void Logging(FILE* _file, CString _csMemo);
};
