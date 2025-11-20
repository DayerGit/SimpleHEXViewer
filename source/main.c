#include <windowsx.h>
#include <stdio.h>
#include "constants.h"
#include "HelpersWinAPI.h"
#include "SearchStructure.h"
#include "WindowLocalStruct.h"
#include "WindowsProc.h"
#include "DPI.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&icex);

	SetProcessDPIAware();

	HMONITOR hPrimaryMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
	UINT currentDPI = GetMonitorDPI(hPrimaryMonitor);

	WindowLocalStruct wls = { 0 };
	SearchStructure ss = { 0 };
	CreateClassExW(L"MainWindowClass", CreateSolidBrush(RGB(255, 255, 255)), IDC_ARROW, DefMainWindowProc, sizeof(WindowLocalStruct*));
	CreateClassExW(L"SearchWindowClass", CreateSolidBrush(RGB(255, 255, 255)), IDC_ARROW, DefSearchWindowProc, sizeof(SearchStructure*));

	HWND hWindow = CreateWindowExW(WS_EX_ACCEPTFILES, L"MainWindowClass", L"Hex Viewer", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, ScaleForDPI(10, currentDPI), ScaleForDPI(10, currentDPI), ScaleForDPI(855, currentDPI), ScaleForDPI(510, currentDPI), 0, 0, 0, 0);
	wls.hListView = CreateWindowExW(NULL, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_NOCOLUMNHEADER | LVS_SINGLESEL, ScaleForDPI(0, currentDPI), ScaleForDPI(10, currentDPI), ScaleForDPI(845, currentDPI), ScaleForDPI(420, currentDPI), hWindow, NULL, NULL, NULL);
	wls.hBinView = CreateWindowExW(NULL, L"static", L"", WS_CHILD | WS_VISIBLE, ScaleForDPI(5, currentDPI), ScaleForDPI(433, currentDPI), ScaleForDPI(200, currentDPI), ScaleForDPI(20, currentDPI), hWindow, 0, 0, 0);
	
	wls.hSearchWindow = CreateWindowExW(NULL, L"SearchWindowClass", L"Поиск", WS_POPUP | WS_CAPTION | WS_SYSMENU, ScaleForDPI(10, currentDPI), ScaleForDPI(10, currentDPI), ScaleForDPI(400, currentDPI), ScaleForDPI(180, currentDPI), hWindow, 0, 0, 0);
	ss.hEdit = CreateWindowExW(NULL, L"edit", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_BORDER, ScaleForDPI(0, currentDPI), ScaleForDPI(5, currentDPI), ScaleForDPI(390, currentDPI), ScaleForDPI(50, currentDPI), wls.hSearchWindow, 0, 0, 0);
	ss.hCheckBox = CreateWindowExW(NULL, L"Button", L"Искать как HEX", BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | BS_CENTER, ScaleForDPI(10, currentDPI), ScaleForDPI(60, currentDPI), ScaleForDPI(150, currentDPI), ScaleForDPI(30, currentDPI), wls.hSearchWindow, BUTTON_FIND_AS_HEX, 0, 0);
	HWND hSearchButton = CreateWindowExW(NULL, L"Button", L"Поиск", WS_CHILD | WS_VISIBLE | BS_CENTER, ScaleForDPI(240, currentDPI), ScaleForDPI(90, currentDPI), ScaleForDPI(120, currentDPI), ScaleForDPI(40, currentDPI), wls.hSearchWindow, BUTTON_SEARCH, 0, 0);

	wls.hMenu = CreateMenu();
	wls.hMenuFile = CreatePopupMenu();
	wls.hMenuEdit = CreatePopupMenu();
	wls.hMenuAbout = CreatePopupMenu();

	AppendMenuW(wls.hMenu, MF_STRING | MF_POPUP, wls.hMenuFile, L"Файл");
	AppendMenuW(wls.hMenuFile, MF_STRING, MENU_OPEN, L"Открыть");
	AppendMenuW(wls.hMenuFile, MF_SEPARATOR, 0, L"");
	AppendMenuW(wls.hMenuFile, MF_STRING, MENU_EXIT, L"Выход");

	AppendMenuW(wls.hMenu, MF_STRING | MF_POPUP, wls.hMenuEdit, L"Редактирование");
	AppendMenuW(wls.hMenuEdit, MF_STRING | MF_DISABLED, MENU_COPY_HEX, L"Копировать HEX");
	AppendMenuW(wls.hMenuEdit, MF_STRING | MF_DISABLED, MENU_COPY_ASCII, L"Копировать ASCII");
	AppendMenuW(wls.hMenuEdit, MF_SEPARATOR, 0, L"");
	AppendMenuW(wls.hMenuEdit, MF_STRING | MF_DISABLED, MENU_SEARCH, L"Поиск");

	AppendMenuW(wls.hMenu, MF_STRING | MF_POPUP, wls.hMenuAbout, L"О программе");
	AppendMenuW(wls.hMenuAbout, MF_STRING, MENU_HELP_ME_PLS, L"Что это?");

	SetMenu(hWindow, wls.hMenu);

	LVCOLUMNW col = { 0 };
	col.mask = LVCF_WIDTH | LVCF_FMT;
	col.fmt = LVCFMT_LEFT;

	col.cx = ScaleForDPI(80, currentDPI);
	ListView_InsertColumn(wls.hListView, 0, &col);

	col.cx = ScaleForDPI(27, currentDPI);
	for (int i = 1; i < 17; i++) {
		if (i == 16) col.cx = ScaleForDPI(35, currentDPI);
		ListView_InsertColumn(wls.hListView, i, &col);
	}

	col.cx = ScaleForDPI(19, currentDPI);
	for (int i = 17; i < 33; i++)
		ListView_InsertColumn(wls.hListView, i, &col);

	HFONT font = CreateFontW(ScaleForDPI(15, currentDPI), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");

	SetWindowFont(wls.hListView, font, TRUE);
	SetWindowFont(wls.hBinView, font, TRUE);
	SetWindowFont(ss.hEdit, font, TRUE);
	SetWindowFont(ss.hCheckBox, font, TRUE);
	SetWindowFont(hSearchButton, font, TRUE);
	
	SetWindowLongPtrW(hWindow, GWLP_USERDATA, &wls);
	SetWindowLongPtrW(wls.hSearchWindow, GWLP_USERDATA, &ss);
	wls.pOldListViewProc = SetWindowLongPtrW(wls.hListView, GWLP_WNDPROC, ListViewWindowProcW);
	ShowWindow(hWindow, SW_SHOWNORMAL);

	MSG message;
	while (GetMessageW(&message, 0, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
	DeleteObject(font);
	return 0;
}