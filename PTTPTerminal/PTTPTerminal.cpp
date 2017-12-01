// PTTPTerminal.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PTTPTerminal.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ReadDataFromPort(LPVOID lpParam);

//Main window handle
HWND hWnd;

// Status bar handle
HWND hSB;

// Serial port globals
HANDLE hSerialPort;
LPWSTR portName = TEXT("COM6");
COMMCONFIG cc;

// Read thread globals
HANDLE hReadThread;
BOOL bReading;

// File dialog globals
OPENFILENAME ofn;
HANDLE file;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	hSerialPort = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (hSerialPort == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("Error opening serial port"), NULL, MB_OK);
		return FALSE;
	}

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PTTPTERMINAL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

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
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PTTPTERMINAL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PTTPTERMINAL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
			case IDM_SETTINGS:
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
				if (GetOpenFileName(&ofn))
				{
					file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if (file == INVALID_HANDLE_VALUE)
					{
						MessageBox(NULL, TEXT("Error opening file"), NULL, MB_OK);
						return FALSE;
					}
					// packetize file
					HDC mainHdc = GetDC(hWnd);
					TextOut(mainHdc, 10, 10, fileName, _tcslen(fileName)); // works!!

					// write file to serial port
				} 
				else
				{
					
				}
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
	case WM_CREATE:
		{
			hSB = CreateStatusBar(hWnd, StatusBar::Id, hInst, StatusBar::NumParts);
			break;
		}
	case WM_SIZE:
		{
			ResizeStatusBar(&hSB);
			break;
		}
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

DWORD WINAPI ReadDataFromPort(LPVOID lpParam)
{
	BYTE inBuff[100];
	DWORD nBytesRead, dwEvent, dwError, dwLastError;
	COMSTAT cs;
	if (!SetCommMask(hSerialPort, EV_RXCHAR)) {
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
