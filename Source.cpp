#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef UNICODE
#define UNICODE
#endif

#define _WIN32_WINNT 0x0500
#define PI 3.1415926535897932386

#include <windows.h>
#include <math.h>
#include <stdio.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static POINT point;
	static POINT pointOld;
	static BOOL bCapture;
	static HFONT hFont;
	static TCHAR szText[1024];
	switch (msg)
	{
	case WM_CREATE:
		hFont = CreateFont(64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("メイリオ"));
		break;
	case WM_LBUTTONDOWN:
		if (!bCapture)
		{
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			SetCapture(hWnd);
			bCapture = 1;
		}
		break;
	case WM_RBUTTONUP:
		if (bCapture)
		{
			ReleaseCapture();
			bCapture = 0;
		}
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		const HDC hdc = BeginPaint(hWnd, &ps);
		const HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
		RECT rect;
		GetClientRect(hWnd, &rect);
		DrawText(hdc, szText, -1, &rect, DT_WORDBREAK);
		SelectObject(hdc, hFontOld);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOUSEMOVE:
		if (bCapture)
		{
			const HDC hdc = GetDC(hWnd);
			const HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			const HPEN hPenOld = (HPEN)SelectObject(hdc, hPen);
			MoveToEx(hdc, point.x, point.y, 0);
			LineTo(hdc, pointOld.x, pointOld.y);
			SelectObject(hdc, hPenOld);
			DeleteObject(hPen);
			MoveToEx(hdc, point.x, point.y, 0);
			LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
			const HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
			const double dAngle = atan2((double)(point.x - LOWORD(lParam)), (double)(point.y - HIWORD(lParam)));
			const double dDistance = sqrt((double)((point.x - LOWORD(lParam))*(point.x - LOWORD(lParam)) + (point.y - HIWORD(lParam))*(point.y - HIWORD(lParam))));
			swprintf_s(szText, 1024,
				TEXT("角度 %f 度\r\n")
				TEXT("角度 %f ラジアン\r\n")
				TEXT("距離 %f ピクセル\r\n"),
				(dAngle + PI / 2.0)*180.0 / PI,
				dAngle + PI / 2.0,
				dDistance + 1.0
				);
			RECT rect;
			GetClientRect(hWnd, &rect);
			DrawText(hdc, szText, -1, &rect, DT_WORDBREAK);
			SelectObject(hdc, hFontOld);
			ReleaseDC(hWnd, hdc);
			pointOld.x = LOWORD(lParam);
			pointOld.y = HIWORD(lParam);
		}
		break;
	case WM_LBUTTONUP:
		if (bCapture)
		{
			ReleaseCapture();
			bCapture = 0;
			InvalidateRect(hWnd, 0, 1);
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		else if (wParam == 'C')
		{
			if (GetKeyState(VK_CONTROL)<0)
			{
				const int nLen = lstrlen(szText);
				const HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, sizeof(TCHAR)*(nLen + 1));
				const LPTSTR lpszBuf = (LPTSTR)GlobalLock(hMem);
				lstrcpy(lpszBuf, szText);
				GlobalUnlock(hMem);
				OpenClipboard(NULL);
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, hMem);
				CloseClipboard();
			}
		}
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
	LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		0,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST,
		szClassName,
		0,
		WS_POPUP,
		0,
		0,
		0,
		0,
		0,
		0,
		hInstance,
		0
		);
	SetLayeredWindowAttributes(
		hWnd,
		0,
		128,
		LWA_ALPHA);
	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
