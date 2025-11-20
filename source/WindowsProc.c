#define _CRT_SECURE_NO_WARNINGS

#include "WindowsProc.h"
#include "constants.h"
#include "WindowLocalStruct.h"
#include "SearchStructure.h"
#include "OpenHexFile.h"
#include <windowsx.h>
#include <strsafe.h>


LRESULT WINAPI DefMainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	WindowLocalStruct* wls = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	static int selectedItem;
	static int selectedSubItem;
	switch (msg) {
	case WM_DROPFILES: {
		wchar_t buf[MAX_PATH] = { 0 };
		DragQueryFileW(wparam, 0, buf, MAX_PATH);

		OpenFileForHEX(buf, wls);
		break;
	}
	case WM_NOTIFY: {
		NMHDR* hdr = (NMHDR*)lparam;
		switch (hdr->code) {
		case NM_CUSTOMDRAW: {
			NMLVCUSTOMDRAW* lpCustomDraw = (NMLVCUSTOMDRAW*)lparam;

			if (lpCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT) {
				return CDRF_NOTIFYITEMDRAW;
			}

			if (lpCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
				return CDRF_NOTIFYSUBITEMDRAW;
			}

			if (lpCustomDraw->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
				int itemIndex = lpCustomDraw->nmcd.dwItemSpec;
				int subItemIndex = lpCustomDraw->iSubItem;

				BOOL isSelected = FALSE;

				for (int i = 0; i < wls->selectedCount; i++) {
					if (wls->selectedBytes[i].y == itemIndex && wls->selectedBytes[i].x == subItemIndex
						|| (wls->selectedBytes[i].y == itemIndex && (wls->selectedBytes[i].x + 16) == subItemIndex)
						|| (wls->selectedBytes[i].y == itemIndex && (wls->selectedBytes[i].x - 16) == subItemIndex)) {
						isSelected = TRUE;
						break;
					}
				}

				if (isSelected && subItemIndex > 0 && subItemIndex < 17) {
					lpCustomDraw->clrText = RGB(255, 255, 255);
					lpCustomDraw->clrTextBk = (wls->isHex ? RGB(0, 0, 255) : RGB(187, 212, 235));
				}
				else if (isSelected && subItemIndex >= 17) {
					lpCustomDraw->clrText = RGB(255, 255, 255);
					lpCustomDraw->clrTextBk = (wls->isHex ? RGB(187, 212, 235) : RGB(0, 0, 255));
				}
				else {
					lpCustomDraw->clrText = RGB(0, 0, 0);
					lpCustomDraw->clrTextBk = RGB(255, 255, 255);
				}

				return CDRF_NEWFONT;
			}
			break;
		}
		case LVN_GETDISPINFO: {
			NMLVDISPINFOW* lpDispInfo = (NMLVDISPINFOW*)lparam;
			int itemIndex = lpDispInfo->item.iItem;
			DWORD64 dwOffset = itemIndex * 16;
			int bytesInLine = min(16, wls->dwFileSize.QuadPart - dwOffset);
			int i = 0;

			if (lpDispInfo->item.mask & LVIF_TEXT) {
				switch (lpDispInfo->item.iSubItem) {
				case 0: {
					StringCchPrintfW(lpDispInfo->item.pszText, lpDispInfo->item.cchTextMax, L"%08X", itemIndex * 16);
					break;
				}

				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16: {
					int i = lpDispInfo->item.iSubItem - 1;
					if (i < bytesInLine)
						StringCchPrintfW(lpDispInfo->item.pszText, lpDispInfo->item.cchTextMax, L"%02X", ((BYTE*)wls->pData)[dwOffset + i]);
					break;
				}

				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 24:
				case 25:
				case 26:
				case 27:
				case 28:
				case 29:
				case 30:
				case 31:
				case 32: {
					int i = lpDispInfo->item.iSubItem - 17;
					if (i < bytesInLine) {
						BYTE b = ((BYTE*)wls->pData)[dwOffset + i];
						lpDispInfo->item.pszText[0] = (b >= 32 && b < 127) ? (wchar_t)b : L'.';
						lpDispInfo->item.pszText[1] = 0;
					}
					break;
				}
				}
			}
			break;
		}
		}
		break;
	}
	case WM_CTLCOLORSTATIC: return GetStockObject(WHITE_BRUSH);
	case WM_COMMAND: {
		switch (wparam) {
		case MENU_OPEN: {
			wchar_t path[MAX_PATH] = { 0 };

			OPENFILENAMEW openFile = { 0 };
			openFile.lStructSize = sizeof(OPENFILENAMEW);
			openFile.hwndOwner = hwnd;
			openFile.lpstrFilter = L"Все файлы\0*.*\0";
			openFile.lpstrFile = path;
			openFile.nMaxFile = MAX_PATH;
			openFile.lpstrTitle = L"Открыть файл\0";
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

			if (GetOpenFileNameW(&openFile))
				OpenFileForHEX(path, wls);
			break;
		}
		case MENU_HELP_ME_PLS: {
			MessageBoxW(hwnd, L"HEX Viewer\n\nПростой HEX-просмотрщик, созданный Dayer для работы над более крупным проектом :)\n\nВерсия 1.0", L"Что это?", MB_ICONQUESTION);
			break;
		}
		case MENU_COPY_ASCII:
		case MENU_COPY_HEX: {
			LVITEM item;
			item.mask = LVIF_TEXT;

			wchar_t tempBuf[3];
			item.pszText = tempBuf;
			item.cchTextMax = 3;

			size_t sizeOfMem = (wls->selectedCount * ((wparam == MENU_COPY_HEX) + 1) + 1) * sizeof(wchar_t);

			wchar_t* buf = calloc(1, sizeOfMem);

			HGLOBAL hGlobalMem = GlobalAlloc(GMEM_MOVEABLE, sizeOfMem);
			if (!hGlobalMem) {
				free(buf);
				break;
			}

			for (int i = 0; i < wls->selectedCount; i++) {
				item.iItem = wls->selectedBytes[i].y;
				if (MENU_COPY_HEX == wparam) item.iSubItem = wls->selectedBytes[i].x - (!wls->isHex * 16);
				else item.iSubItem = wls->selectedBytes[i].x + (wls->isHex * 16);

				if (ListView_GetItem(wls->hListView, &item))
					wcscat(buf, tempBuf);
			}

			wchar_t* globalPtr = (wchar_t*)GlobalLock(hGlobalMem);
			if (globalPtr) {
				wcscpy_s(globalPtr, sizeOfMem / sizeof(wchar_t), buf);
				GlobalUnlock(hGlobalMem);
			}

			free(buf);

			if (OpenClipboard(hwnd)) {
				EmptyClipboard();

				if (!SetClipboardData(CF_UNICODETEXT, hGlobalMem))
					GlobalFree(hGlobalMem);
				CloseClipboard();
			}
			else GlobalFree(hGlobalMem);
			break;
		}
		case MENU_SEARCH: {
			ShowWindow(wls->hSearchWindow, SW_SHOWNORMAL);
			break;
		}
		case MENU_EXIT: PostQuitMessage(0);
		}
		break;
	}
	case WM_DESTROY: {
		if (wls->pData)
			UnmapViewOfFile(wls->pData);

		if (wls->hMap)
			CloseHandle(wls->hMap);

		if (wls->hFile)
			CloseHandle(wls->hFile);

		PostQuitMessage(0);
	}
	default: return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
	return 0;
}

LRESULT WINAPI DefSearchWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	WindowLocalStruct* wls = GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA);
	SearchStructure* ss = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	switch (msg) {
	case WM_COMMAND: {
		switch (wparam) {
		case BUTTON_FIND_AS_HEX: {
			wls->isHex = Button_GetCheck(lparam);
			break;
		}
		case BUTTON_SEARCH: {
			int searchLen = GetWindowTextLengthA(ss->hEdit);
			if (searchLen <= 0) break;

			char* search = malloc(searchLen + 1);
			GetWindowTextA(ss->hEdit, search, searchLen + 1);

			if (!search) break;

			char* actualSearch = search;
			int actualSearchLen = searchLen;

			if (wls->isHex) {
				if (searchLen % 2 != 0) {
					MessageBoxW(hwnd, L"HEX строка должна иметь четную длину!", L"Ошибка!", MB_ICONERROR);
					free(search);
					break;
				}

				actualSearchLen = searchLen / 2;
				actualSearch = (char*)malloc(actualSearchLen + 1);

				for (int i = 0, j = 0; i < searchLen; i += 2, j++) {
					while (search[i] == ' ' && i < searchLen) i++;
					if (i >= searchLen) break;

					char hexPair[3] = { search[i], search[i + 1], '\0' };
					char* endptr;
					long value = strtol(hexPair, &endptr, 16);

					if (endptr == hexPair || *endptr != '\0') {
						MessageBoxW(hwnd, L"Некорректный HEX формат!", L"Ошибка!", MB_ICONERROR);
						free(search);
						free(actualSearch);
						break;
					}

					actualSearch[j] = (char)value;
				}
				actualSearch[actualSearchLen] = 0;

				free(search);
				search = NULL;
			}

			if (actualSearchLen <= 0 || actualSearchLen > wls->dwFileSize.QuadPart) {
				if (wls->isHex) free(actualSearch);
				else free(search);
				break;
			}

			char table[MAX_PATH];
			memset(table, actualSearchLen, MAX_PATH);

			for (int i = 0; i < actualSearchLen - 1; i++)
				table[(unsigned char)actualSearch[i]] = actualSearchLen - i - 1;

			char* dataStart = (char*)wls->pData;
			char* dataEnd = dataStart + wls->dwFileSize.QuadPart;

			char* current = dataStart + actualSearchLen - 1 + ss->searchShift;
			char* foundPos = NULL;

			while (current < dataEnd) {
				if (*current == actualSearch[actualSearchLen - 1]) {
					BOOL isFound = TRUE;
					for (int i = 0; i < actualSearchLen; i++) {
						if (actualSearch[actualSearchLen - 1 - i] != *(current - i)) {
							isFound = FALSE;
							break;
						}
					}

					if (isFound) {
						foundPos = current - (actualSearchLen - 1);
						break;
					}
				}

				char shift = table[(char)*current];
				current += shift;
			}

			if (foundPos) {
				LONGLONG pos = foundPos - dataStart;
				ss->searchShift = pos + 1;
				wls->selectedCount = 1;
				if (wls->selectedBytes)
					free(wls->selectedBytes);

				wls->selectedBytes = malloc(sizeof(POINT));
				wls->selectedBytes->y = pos / 16;
				wls->selectedBytes->x = pos % 16 + 1;

				InvalidateRect(wls->hListView, 0, 0);
				ShowWindow(hwnd, SW_HIDE);
			}
			else {
				MessageBoxW(hwnd, L"Вхождение не найдено!", L"Ошибка поиска!", MB_ICONERROR);
				ss->searchShift = 0;
			}

			if (wls->isHex) free(actualSearch);
			else free(search);

			break;
		}
		}
	}
	case WM_CTLCOLORSTATIC: return GetStockObject(WHITE_BRUSH);
	default: return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
	return 0;
}

LRESULT ListViewWindowProcW(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	WindowLocalStruct* wls = GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA);
	switch (msg) {
	case WM_LBUTTONDOWN: {
		LVHITTESTINFO hitTestInfo = { 0 };
		hitTestInfo.pt.x = GET_X_LPARAM(lparam);
		hitTestInfo.pt.y = GET_Y_LPARAM(lparam);
		ListView_SubItemHitTest(hwnd, &hitTestInfo);

		if (hitTestInfo.flags & LVHT_ONITEM) {
			wls->startPoint.x = hitTestInfo.iSubItem;
			wls->startPoint.y = hitTestInfo.iItem;

			wls->isHex = wls->startPoint.x <= 16;

			if (wls->selectedBytes) {
				free(wls->selectedBytes);
				wls->selectedBytes = NULL;
			}
			wls->selectedCount = 0;
		}
		break;
	}
	case WM_LBUTTONUP: {
		LVHITTESTINFO hitTestInfo = { 0 };
		hitTestInfo.pt.x = GET_X_LPARAM(lparam);
		hitTestInfo.pt.y = GET_Y_LPARAM(lparam);
		ListView_SubItemHitTest(hwnd, &hitTestInfo);

		if ((hitTestInfo.flags & LVHT_ONITEM) && wls->startPoint.x != -1 && wls->startPoint.y != -1) {
			int startRow = wls->startPoint.y;
			int startCol = wls->startPoint.x;
			int endRow = hitTestInfo.iItem;
			int endCol = hitTestInfo.iSubItem;
			int currentRow, rowStartCol, rowEndCol, startX, endX;

			int rowStep = startRow <= endRow ? 1 : -1;

			wls->selectedCount = 0;
			int rowCount = abs(endRow - startRow) + 1;

			for (int i = 0; i < rowCount; i++) {
				currentRow = startRow + (i * rowStep);

				if (wls->isHex) {
					rowStartCol = (currentRow == startRow) ? startCol : 1;
					rowEndCol = (currentRow == endRow) ? endCol : 16;
				}
				else {
					rowStartCol = (currentRow == startRow) ? startCol : 17;
					rowEndCol = (currentRow == endRow) ? endCol : 32;
				}

				startX = min(rowStartCol, rowEndCol);
				endX = max(rowStartCol, rowEndCol);
				wls->selectedCount += (endX - startX + 1);
			}

			if (wls->selectedBytes)
				free(wls->selectedBytes);

			wls->selectedBytes = malloc(wls->selectedCount * sizeof(POINT));

			int index = 0;
			for (int i = 0; i < rowCount; i++) {
				currentRow = startRow + (i * rowStep);

				if (wls->isHex) {
					rowStartCol = (currentRow == startRow) ? startCol : 1;
					rowEndCol = (currentRow == endRow) ? endCol : 16;
				}
				else {
					rowStartCol = (currentRow == startRow) ? startCol : 17;
					rowEndCol = (currentRow == endRow) ? endCol : 32;
				}

				startX = min(rowStartCol, rowEndCol);
				endX = max(rowStartCol, rowEndCol);

				for (int x = startX; x <= endX; x++) {
					wls->selectedBytes[index].x = x;
					wls->selectedBytes[index].y = currentRow;
					index++;
				}
			}

			if (wls->selectedCount == 1) {
				char c = ((char*)wls->pData)[wls->selectedBytes[0].y * 16 + (wls->selectedBytes[0].x % 16) - 1];
				wchar_t buf[15] = { 'B', 'i', 'n', ':', ' ', 0 };

				for (int i = 7, j = 5; i >= 0; i--, j++) {
					buf[j] = ((c >> i) & 1) + L'0';
				}

				SetWindowTextW(wls->hBinView, buf);
			}

			InvalidateRect(hwnd, NULL, FALSE);
			wls->startPoint.x = -1;
			wls->startPoint.y = -1;

			for (int i = MENU_COPY_HEX; i < MENU_SEARCH; i++)
				EnableMenuItem(wls->hMenuEdit, i, MF_ENABLED);
		}
		break;
	}
	default: break;
	}
	return CallWindowProcW(wls->pOldListViewProc, hwnd, msg, wparam, lparam);
}