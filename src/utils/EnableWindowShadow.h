#pragma once
#include <windows.h>
#include <dwmapi.h>
#include <windowsx.h>

class EnableWindowShadow {
public:
    static void apply(HWND hwnd);
private:
    static LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static WNDPROC originalWndProc;
};