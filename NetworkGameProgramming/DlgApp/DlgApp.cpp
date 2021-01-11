#include <Windows.h>
#include <stdio.h>
#include "resource.h"

#define BUFSIZE	25

// 대화상자 프로시저
bool CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(char* fmt, ...);

HWND hEdit1, hEdit2; // 편집 컨트롤

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DlgProc);

	return 0;
}

// 대화상자 프로시저
bool CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static char buf[BUFSIZE + 1];

	switch (uMsg)
	{
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
			DisplayText("%s\r\n", buf);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			return true;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return true;
		}
		return false;
	}
	return false;
}

// 편집 컨트롤 출력 함수
void DisplayText(char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf_s(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);

	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, false, (LPARAM)cbuf);

	va_end(arg);
}