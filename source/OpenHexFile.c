#include "OpenHexFile.h"
#include "constants.h"
#include <commctrl.h>

void OpenFileForHEX(wchar_t* path, WindowLocalStruct* wls) {
	if (wls->pData)
		UnmapViewOfFile(wls->pData);

	if (wls->hMap)
		CloseHandle(wls->hMap);

	if (wls->hFile)
		CloseHandle(wls->hFile);

	wls->hFile = CreateFileW(path, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == wls->hFile) DisplayError();

	if (!GetFileSizeEx(wls->hFile, &wls->dwFileSize)) DisplayError();

	wls->hMap = CreateFileMappingW(wls->hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!wls->hMap) DisplayError();

	wls->pData = MapViewOfFile(wls->hMap, FILE_MAP_READ, 0, 0, 0);
	if (!wls->pData) DisplayError();

	ListView_SetItemCount(wls->hListView, (wls->dwFileSize.QuadPart / (LONGLONG)16) + (LONGLONG)1);
	EnableMenuItem(wls->hMenuEdit, MENU_SEARCH, MF_ENABLED);
}
