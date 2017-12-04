#pragma once

namespace StatusBar
{
	static constexpr int Id = 100;
	static constexpr int NumParts = 2;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: CreateStatusBar
--
-- DATE: December 1, 2017
--
-- REVISIONS:
--
-- DESIGNER: Will Murphy
--
-- PROGRAMMER: Will Murphy
--
-- INTERFACE: HWND CreateStatusBar(HWND hwndParent, int idStatus, HINSTANCE hinst, int cParts)
--								HWND hwndParent: The parent window.
--								int idStatus: The ID of the status bar.
--								HINSTANCE hinst: The application instance.
--								int cParts: The number of parts the status bar has.
--
-- RETURNS: Returns a handle to the status bar window, whose parent is the application window.
--
----------------------------------------------------------------------------------------------------------------------*/
inline HWND CreateStatusBar(HWND hwndParent, int idStatus, HINSTANCE hinst, int cParts)
{
	HWND hwndStatus;
	RECT rcClient;
	HLOCAL hloc;
	PINT paParts;
	int nWidth;

	INITCOMMONCONTROLSEX ix;
	ix.dwSize = sizeof(INITCOMMONCONTROLSEX);
	ix.dwICC = ICC_WIN95_CLASSES;
	// Ensure that the common control DLL is loaded.
	InitCommonControlsEx(&ix);

	// Create the status bar.
	hwndStatus = CreateWindowEx(
		0,                       // no extended styles
		STATUSCLASSNAME,         // name of status bar class
		(PCTSTR)NULL,           // no text when first created
		SBARS_SIZEGRIP |         // includes a sizing grip
		WS_CHILD | WS_VISIBLE,   // creates a visible child window
		0, 0, 0, 0,              // ignores size and position
		hwndParent,              // handle to parent window
		(HMENU)idStatus,       // child window identifier
		hinst,                   // handle to application instance
		NULL);                   // no window creation data

								 // Get the coordinates of the parent window's client area.
	GetClientRect(hwndParent, &rcClient);

	// Allocate an array for holding the right edge coordinates.
	hloc = LocalAlloc(LHND, sizeof(int) * cParts);
	paParts = (PINT)LocalLock(hloc);

	// Calculate the right edge coordinate for each part, and
	// copy the coordinates to the array.
	nWidth = rcClient.right / cParts;
	int rightEdge = nWidth;
	for (int i = 0; i < cParts; i++) {
		paParts[i] = rightEdge;
		rightEdge += nWidth;
	}

	// Tell the status bar to create the window parts.
	SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)cParts, (LPARAM)paParts);
	//SendMessage(hwndStatus, WM_SIZE, 0, 0);
	
	// Free the array, and return.
	LocalUnlock(hloc);
	LocalFree(hloc);
	return hwndStatus;
}


inline int ResizeStatusBar(HWND* hSb)
{
	RECT rcClient;
	GetClientRect(*hSb, &rcClient);
	const int nWidth = rcClient.right / StatusBar::NumParts;
	int rightEdge = nWidth;
	const int cParts = StatusBar::NumParts;
	int paParts[cParts];
	for (int i = 0; i < cParts; i++) {
		paParts[i] = rightEdge;
		rightEdge += nWidth;
	}
	SendMessage(*hSb, SB_SETPARTS, cParts, (LPARAM)paParts);
	SendMessage(*hSb, WM_SIZE, 0, 0);
	return 0;
}