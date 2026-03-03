#include "main.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <shobjidl.h>
#include "EnableWindowShadow.h"
#endif

int main(int argc, char **argv) {
    #ifdef _WIN32
        SetCurrentProcessExplicitAppUserModelID(L"MyCompany.MyApp.ID");
    #endif
    auto ui = AppWindow::create();

    #ifdef _WIN32
        EnableWindowShadow windowutils;
        AppWindow* ptr = &(*ui);
        slint::invoke_from_event_loop([&]() {
            HWND hwnd = ui->window().win32_hwnd();
            if (hwnd) {
                windowutils.apply(hwnd,ptr);
            }
            slint::PhysicalSize winSize = ui->window().size();
            // 2. 获取屏幕主显示器的物理宽度和高度
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            // 3. 计算居中的物理坐标 (X, Y)
            // 这里的 winSize.width 和 winSize.height 就是 PhysicalSize 里的成员
            int x = (screenWidth - (int)winSize.width) / 2;
            int y = (screenHeight - (int)winSize.height) / 2;
            // 4. 调用 set_position 的物理版本进行设置
            ui->window().set_position(slint::PhysicalPosition({ x, y }));
        });
    #endif

    // 绑定 UI 中的回调
    ui->on_request_logic_exec([&](slint::SharedString msg) {
        std::string input = msg.data();
        std::string response = input;
        ui->set_message(response.c_str());
    });
    ui->on_request_minimize([&]() {
        #ifdef _WIN32
                HWND hwnd = ui->window().win32_hwnd();
                if (hwnd) {
                    // SW_MINIMIZE 将窗口缩小到任务栏
                    ShowWindow(hwnd, SW_MINIMIZE);
                }
        #endif
            });

    ui->on_request_close([&]() {
        #ifdef _WIN32
                HWND hwnd = ui->window().win32_hwnd();
                if (hwnd) {
                    // 让窗口走标准的销毁流程，触发 WM_CLOSE
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
        #endif
        });

    ui->on_request_maximize([&]() {
        #ifdef _WIN32
            HWND hwnd = ui->window().win32_hwnd();
            if (hwnd) {
            if (IsZoomed(hwnd)) {
                PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
            } else {
                PostMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
            
            }
        }    
        #endif
    });

    ui->run();
    return 0;
}
