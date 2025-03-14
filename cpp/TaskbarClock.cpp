#define UNICODE
#define _UNICODE
#include <windows.h>
#include <shellapi.h>
#include <time.h>
#include <string>
#include <sstream>
#include <tchar.h>
#include "resource.h"

// 全局变量，存储程序运行时的关键状态和句柄
HINSTANCE g_hInstance;
NOTIFYICONDATA g_nid;
HMENU g_hMenu;
bool g_autoStart = false;

// 函数声明，告诉编译器这些函数的存在和参数类型
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateTrayIcon(HWND hwnd);
void CreateTrayMenu(HWND hwnd);
void ToggleAutoStart();
bool IsAutoStartEnabled();
void SetAutoStart(bool enable);

// 程序入口函数，Windows 应用程序的主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    g_hInstance = hInstance;

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("TaskbarClockClass");
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, TEXT("Window registration failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0,
        TEXT("TaskbarClockClass"),
        TEXT("Taskbar Clock"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 300,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        MessageBox(NULL, TEXT("Window creation failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ZeroMemory(&g_nid, sizeof(g_nid));
    g_nid.cbSize = sizeof(NOTIFYICONDATA);
    g_nid.hWnd = hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_USER + 1;
    g_nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    _tcscpy_s(g_nid.szTip, TEXT("Taskbar Clock"));
    Shell_NotifyIcon(NIM_ADD, &g_nid);


    g_autoStart = IsAutoStartEnabled();

    CreateTrayMenu(hwnd);
    SetTimer(hwnd, 1, 1000, NULL);

    UpdateTrayIcon(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIcon(NIM_DELETE, &g_nid);

    return (int)msg.wParam;
}

// 窗口消息处理回调函数，处理各种系统和用户消息
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_TIMER:
            UpdateTrayIcon(hwnd);
            break;

        case WM_USER + 1:
            switch (lParam) {
                case WM_LBUTTONUP:

                    ShellExecute(NULL, TEXT("open"), TEXT("https://blog.luckly-mjw.cn/tool-show/calendar-web/index.html"), NULL, NULL, SW_SHOWNORMAL);
                    break;

                case WM_RBUTTONUP:
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(hwnd);
                    
                    CheckMenuItem(g_hMenu, 0, MF_BYPOSITION | (g_autoStart ? MF_CHECKED : MF_UNCHECKED));
                    ModifyMenu(g_hMenu, 0, MF_BYPOSITION | MF_STRING, 1, g_autoStart ? TEXT("Stop Autostart") : TEXT("Set Autostart"));
                    
                    TrackPopupMenu(g_hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                    PostMessage(hwnd, WM_NULL, 0, 0);
                    break;
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    ToggleAutoStart();
                    break;
                case 2:
                    DestroyWindow(hwnd);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// 更新托盘图标的时间和日期
void UpdateTrayIcon(HWND hwnd) {
    time_t now = time(0);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);

    TCHAR timeStr[64];
    TCHAR dateStr[64];
    _tcsftime(timeStr, sizeof(timeStr) / sizeof(TCHAR), TEXT("%H:%M:%S"), &timeinfo);
    _tcsftime(dateStr, sizeof(dateStr) / sizeof(TCHAR), TEXT("%Y-%m-%d"), &timeinfo);

    std::basic_stringstream<TCHAR> ss;
    ss << timeStr << TEXT("\n") << dateStr;
    _tcscpy_s(g_nid.szTip, ss.str().c_str());
    
    g_nid.uFlags = NIF_TIP;
    Shell_NotifyIcon(NIM_MODIFY, &g_nid);
}

// 创建右键菜单
void CreateTrayMenu(HWND hwnd) {
    g_hMenu = CreatePopupMenu();

    AppendMenu(g_hMenu, MF_STRING, 1, g_autoStart ? TEXT("Stop Autostart") : TEXT("Set Autostart"));
    AppendMenu(g_hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(g_hMenu, MF_STRING, 2, TEXT("Exit"));
    
    CheckMenuItem(g_hMenu, 0, MF_BYPOSITION | (g_autoStart ? MF_CHECKED : MF_UNCHECKED));
}

// 切换开机自启动状态
void ToggleAutoStart() {
    g_autoStart = !g_autoStart;
    SetAutoStart(g_autoStart);
    CheckMenuItem(g_hMenu, 0, MF_BYPOSITION | (g_autoStart ? MF_CHECKED : MF_UNCHECKED));
    ModifyMenu(g_hMenu, 0, MF_BYPOSITION | MF_STRING, 1, g_autoStart ? TEXT("Stop Autostart") : TEXT("Set Autostart"));
}

// 检查是否已开启开机自启动
bool IsAutoStartEnabled() {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    TCHAR path[MAX_PATH];
    DWORD size = sizeof(path);
    bool exists = RegQueryValueEx(hKey, TEXT("TaskbarClock"), NULL, NULL, (LPBYTE)path, &size) == ERROR_SUCCESS;
    RegCloseKey(hKey);
    
    g_autoStart = exists;
    return exists;
}

// 设置开机自启动
void SetAutoStart(bool enable) {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        return;
    }

    if (enable) {
        TCHAR path[MAX_PATH];
        GetModuleFileName(NULL, path, MAX_PATH);
        RegSetValueEx(hKey, TEXT("TaskbarClock"), 0, REG_SZ, (BYTE*)path, (_tcslen(path) + 1) * sizeof(TCHAR));
    } else {
        RegDeleteValue(hKey, TEXT("TaskbarClock"));
    }

    RegCloseKey(hKey);
    
    g_autoStart = enable;
}