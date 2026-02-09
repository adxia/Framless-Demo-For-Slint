#include "EnableWindowShadow.h"

WNDPROC EnableWindowShadow::originalWndProc = nullptr;

void EnableWindowShadow::apply(HWND hwnd) {
    if (!hwnd) return;

    // 设置样式：保留系统功能但准备去掉标题栏
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, style | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);

    // 启用 DWM 阴影
    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // 拦截窗口消息 (Subclassing)
    originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);

    // 刷新窗口
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

LRESULT CALLBACK EnableWindowShadow::CustomWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_NCCALCSIZE:
        if (wParam) {
        // 获取窗口位置信息
        NCCALCSIZE_PARAMS* p = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
        
        // 检查窗口是否处于最大化状态
        WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
        if (GetWindowPlacement(hwnd, &wp) && wp.showCmd == SW_MAXIMIZE) {
            // 获取当前显示器的信息（考虑多显示器情况）
            HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
            if (hMonitor) {
                MONITORINFO mi = { sizeof(MONITORINFO) };
                GetMonitorInfo(hMonitor, &mi);
                // 将客户区限制在显示器的可用工作区（rcWork 已经排除了任务栏）
                p->rgrc[0] = mi.rcWork;
            }
            }
            return 0; //拦截默认边框计算
        }
        return 0;

    case WM_NCHITTEST: {
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
        const int buttonZone = scale(156);
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
    }
    return CallWindowProc(originalWndProc, hwnd, uMsg, wParam, lParam);
}