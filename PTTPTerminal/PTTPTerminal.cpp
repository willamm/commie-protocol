// PTTPTerminal.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PTTPTerminal.h"

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ReadDataFromPort(LPVOID lpParam, HWND* hWnd);

// Serial port globals
HANDLE hSerialPort;
TCHAR* portName = TEXT("COM6");
COMMCONFIG cc;
COMMPROP cp;
DCB dcb;
COMMTIMEOUTS ct;

// Read thread globals
HANDLE hReadThread;
BOOL bReading;

// File dialog globals
OPENFILENAME ofn;
HANDLE hFile;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(lpCmdLine);

	static WCHAR szTitle[] = L"PTTP Terminal";
	WNDCLASSEXW wcex;

	if (!hPrevInstance)
	{
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PTTPTERMINAL));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PTTPTERMINAL);
		wcex.lpszClassName = szTitle;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
		RegisterClassExW(&wcex);
	}

	// Perform application initialization:
	HWND hWnd = CreateWindowW(szTitle, L"Terminal", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		return FALSE;
	}

	HWND hSB = CreateStatusBar(hWnd, StatusBar::Id, hInstance, StatusBar::NumParts);
	if (!hSB)
	{
		return FALSE;
	}

	// Serial port initialization
	if (!OpenSerialPort(&hSerialPort, portName))
	{
		//return FALSE;
	}
	ConfigurePortDcb(&hSerialPort, &dcb);
	ConfigurePortProps(&hSerialPort, &cp);
	ConfigurePortTimeouts(&hSerialPort, &ct);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	bReading = FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PTTPTERMINAL));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Find out what this is for
	// 
	// Angus Lam
	//
	// Error due to lack of declaration for GWL_INSTANCE
	// For now, just using the literal value, which is -6
	const HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, -6);
	DWORD lrc;
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_SETTINGS_COMSETTINGS:
			lrc = sizeof(COMMCONFIG);
			GetCommConfig(hSerialPort, &cc, &lrc);
			if (!CommConfigDialog(portName, hWnd, &cc)) {
				break;
			}
			if (!SetCommState(hSerialPort, &cc.dcb)) {
				break;
			}
			break;
		case ID_FILE_OPENTEXTFILE:
			WCHAR fileName[500];
			InitFileStruct(&ofn, hWnd, fileName);
			if (!GetOpenFileName(&ofn))
			{
				return FALSE;
			}
			hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				MessageBox(NULL, TEXT("Error opening file"), NULL, MB_OK);
				return FALSE;
			}
			break;
		case ID_SETTINGS_SELECTCOMPORT:
			// Open Select COM Port dialog
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SIZE:
	{
		HWND hSB = FindWindowEx(hWnd, NULL, STATUSCLASSNAME, NULL);
		ResizeStatusBar(&hSB);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

DWORD WINAPI ReadDataFromPort(LPVOID lpParam, HWND* hWnd)
{
	BYTE inBuff[100];
	DWORD nBytesRead, dwEvent, dwError, dwLastError, dwStoredFlags;
	COMSTAT cs;

	dwStoredFlags = EV_RXCHAR | EV_RXFLAG;
	if (!SetCommMask(hSerialPort, dwStoredFlags)) {
		MessageBox(NULL, TEXT("Error"), NULL, MB_OK);
		return FALSE;
	}
	while (bReading) {
		if (WaitCommEvent(hSerialPort, &dwEvent, NULL)) {
			ClearCommError(hSerialPort, &dwError, &cs);
			if ((dwEvent & EV_RXCHAR) && cs.cbInQue) {
				if (!ReadFile(hSerialPort, inBuff, cs.cbInQue, &nBytesRead, NULL)) {
					dwLastError = GetLastError();
					ClearCommError(hSerialPort, &dwLastError, &cs);
				}
				else {
					if (nBytesRead) {
						WriteFile(hWnd, inBuff, nBytesRead, NULL, NULL);
					}
				}
			}
		}
		else {
			dwLastError = GetLastError();
			ClearCommError(hSerialPort, &dwLastError, &cs);
		}
	}
	PurgeComm(hSerialPort, PURGE_RXCLEAR);
	return 0L;
}

