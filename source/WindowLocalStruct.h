#pragma once
#include <Windows.h>

typedef struct {
	HANDLE hFile, hMap;
	LPVOID pData;
	LARGE_INTEGER dwFileSize;
	DWORD64 dwCurrentOffset;
	HWND hListView, hBinView, hSearchWindow;

	POINT startPoint;
	POINT* selectedBytes;
	int selectedCount;
	WNDPROC pOldListViewProc;
	BOOL isHex;

	HMENU hMenu, hMenuFile, hMenuEdit, hMenuAbout;
} WindowLocalStruct;