#pragma once

#include <atomic>

// CClientSocket 명령 대상
#define WM_CLIENT_RECV WM_USER+1

class CClientSocket : public CSocket
{
public:
	HWND m_hWnd;
	std::atomic<bool> isSocketConnected;

	CClientSocket();
	virtual ~CClientSocket();

	void SetWnd(HWND hWnd);
		
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	BOOL OnMessagePending();
};