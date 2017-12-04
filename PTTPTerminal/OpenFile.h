#pragma once

inline void InitFileStruct(LPOPENFILENAME _Out_ lpOfn, HWND parent, LPWSTR fileName)
{
	ZeroMemory(lpOfn, sizeof(lpOfn));
	fileName[0] = '\0';
	lpOfn->lStructSize = sizeof(OPENFILENAME);
	lpOfn->Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	lpOfn->hwndOwner = parent;
	lpOfn->lpstrFile = fileName;
	lpOfn->nMaxFile = 500; //temp
	lpOfn->lpstrFilter = L"Text Files(*.txt)\0";
	lpOfn->lpstrDefExt = L"txt";
}


