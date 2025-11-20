#pragma once
#include <Windows.h>
#include <commctrl.h>

LRESULT WINAPI DefMainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT WINAPI DefSearchWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT WINAPI ListViewWindowProcW(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);