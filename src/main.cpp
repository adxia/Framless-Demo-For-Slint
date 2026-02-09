#include "main.h"
#ifdef _WIN32
#include "EnableWindowShadow.h"
#include <windows.h>
#endif

int main(int argc, char **argv) {

    auto ui = AppWindow::create();

    #ifdef _WIN32
        slint::invoke_from_event_loop([&]() {
            HWND hwnd = ui->window().win32_hwnd();
            if (hwnd) {
                EnableWindowShadow::apply(hwnd);
            }
        });
    #endif
    
    // 绑定 UI 中的回调
    ui->on_request_logic_exec([&](slint::SharedString msg) {
        std::string input = msg.data();
        std::string response = "C++ 已处理: " + input;

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

            // --- 2. 处理关闭 ---
    ui->on_request_close([&]() {
        #ifdef _WIN32
                HWND hwnd = ui->window().win32_hwnd();
                if (hwnd) {
                    // 让窗口走标准的销毁流程，触发 WM_CLOSE
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
        #else
                // 非 Windows 平台直接退出
                std::exit(0);
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
        #else
         
        #endif
    });
    ui->run();
    return 0;
}
