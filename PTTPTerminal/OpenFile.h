#pragma once

inline UINT_PTR CALLBACK Hook(HWND _In_ hWnd, UINT _In_ uiMsg, WPARAM _In_ wParam, LPARAM  _In_ lParam)
{
	HWND parent = GetParent(hWnd);
	switch (uiMsg)
	{
	case WM_INITDIALOG:
		switch (lParam)
		{
		case CDN_INCLUDEITEM:
			MessageBox(parent, TEXT("Msg"), NULL, 0);
			return NULL;
		}
	case CDM_SETDEFEXT:
		CommDlg_OpenSave_SetDefExt(hWnd, "txt");

	}
	return NULL;
}

inline void WINAPI InitFileStruct(LPOPENFILENAME _Out_ lpOfn, HWND parent)
{
	ZeroMemory(lpOfn, sizeof(lpOfn));
	lpOfn->lStructSize = sizeof(OPENFILENAME);
	lpOfn->Flags = OFN_ENABLEHOOK | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	lpOfn->hwndOwner = parent;
	lpOfn->nMaxFile = 500; //temp
	lpOfn->lpstrTitle = NULL;
	lpOfn->lpstrFileTitle = NULL; 
	lpOfn->lpstrFilter = L"Text Files(*.txt)\0";
	lpOfn->lpfnHook = &Hook;
}


