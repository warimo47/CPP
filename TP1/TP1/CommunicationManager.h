#pragma once

#include <thread>
#include <atomic>

#pragma pack(push, 1)
struct RepetitionData
{
	char ID[40]; // "2021-06-15 11 00001"
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

class CommunicationManager
{
private:
	std::atomic<bool>* pIsRtspStreaming;

	// TCP / IP
	CSocket m_Socket;
	bool isSocketConnected;
	unsigned char sendBuf[4][4096];
	RepetitionData repetitionData[4];

	// HTTP
	std::thread* getPTZThreads[4];

	// Day
	CString csDay;

protected:

public:
	// TCP / IP
	CString serverIPStr;
	bool communicationStatus;
	int resultSize[4];

	// HTTP
	CString cctvIPs[4];
	std::atomic<int> ptz[4][3];

	// CCTV index
	int cctvIndex[4];

private:

protected:

public:
	CommunicationManager();
	~CommunicationManager();

	void SetTime(int _thn);
	void SetSize(int _thn);
	void SetRepetitionData(int _thn, int _ID, int _objType, float _XAxis, float _ZAxis,
		int _left, int _right, int _top, int _bottom, float _percent);

	// TCP / IP
	bool TryConnectServer();

	CString GetNowTime();

	void SendPacket(int _thn);

	void SocketClose();

	// HTTP
	void StartGetPTZ(int _thn);
	void RequestHTTP(int _thn);
};

