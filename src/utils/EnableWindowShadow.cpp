#include "EnableWindowShadow.h"


WNDPROC EnableWindowShadow::originalWndProc = nullptr;

void EnableWindowShadow::apply(HWND hwnd, AppWindow* ui_ptr) {
    if (!hwnd) return;
    m_ui = ui_ptr;
   
    // 设置样式：保留系统功能但准备去掉标题栏
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, style | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);

    // 启用 DWM 阴影
    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // 拦截窗口消息 (Subclassing)
    SetWindowSubclass(hwnd, SubclassProc, 1, (DWORD_PTR)this);
    //originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);

    // 刷新窗口
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

LRESULT CALLBACK EnableWindowShadow::SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                                                 UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {

    EnableWindowShadow* self = reinterpret_cast<EnableWindowShadow*>(dwRefData);

    switch (uMsg) {
    case WM_SIZE:
        if (self && self->m_ui) {
            if (wParam == SIZE_MAXIMIZED) self->m_ui->set_is_maximized(true);
            else if (wParam == SIZE_RESTORED) self->m_ui->set_is_maximized(false);
        }
        break;

    case WM_NCCALCSIZE:
        if (wParam) {
            NCCALCSIZE_PARAMS* p = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
            if (GetWindowPlacement(hwnd, &wp) && wp.showCmd == SW_MAXIMIZE) {
                HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
                if (hMonitor) {
                    MONITORINFO mi = { sizeof(MONITORINFO) };
                    GetMonitorInfo(hMonitor, &mi);
                    p->rgrc[0] = mi.rcWork;
                }
            }
            return 0;
        }
        break;

    case WM_NCHITTEST:
        {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hwnd, &pt);

        RECT rect;
        GetClientRect(hwnd, &rect);

        UINT dpi = GetDpiForWindow(hwnd);
        auto scale = [dpi](int logicalSize) {
            return MulDiv(logicalSize, dpi, 96);
        };
        const int b = scale(8);     // 对应 Slint 8px 的缩放边缘
        const int titleHeight = scale(40);   // 对应 Slint 40px 的标题栏高度
        const int buttonZone = scale(208);  // 对应 title右边4个按钮 52*4
        const int btnWidth = scale(52); 

        if (!IsZoomed(hwnd)) {
            // 四角判断
            if (pt.x < b && pt.y < b) return HTTOPLEFT;
            if (pt.x > rect.right - b && pt.y < b) return HTTOPRIGHT;
            if (pt.x < b && pt.y > rect.bottom - b) return HTBOTTOMLEFT;
            if (pt.x > rect.right - b && pt.y > rect.bottom - b) return HTBOTTOMRIGHT;

            // 四边判断
            if (pt.x < b) return HTLEFT;
            if (pt.x > rect.right - b) return HTRIGHT;
            if (pt.y < b) return HTTOP;
            if (pt.y > rect.bottom - b) return HTBOTTOM;
        }

        // 标题栏拖拽判断
        if (pt.y < titleHeight) {
            // 只有当鼠标不在右上角按钮区时，才返回 HTCAPTION
            if (pt.x < (rect.right - buttonZone)){
                return HTCAPTION;
            }
        }

        // 其他区域才返回客户区
        return HTCLIENT;
        }
        break;
    case WM_NCDESTROY:
        RemoveWindowSubclass(hwnd, SubclassProc, uIdSubclass);
        break;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}