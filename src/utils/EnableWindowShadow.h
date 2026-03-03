#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commctrl.h> 
#pragma comment(lib, "Comctl32.lib")
#include <dwmapi.h>
#include <windowsx.h>
#include <functional>
#include "main.h"
class AppWindow;


class EnableWindowShadow {
public:
    void apply(HWND hwnd, AppWindow* ui_ptr);

private:
    static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static WNDPROC originalWndProc;
    AppWindow* m_ui;
};