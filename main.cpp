#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <iostream>
#include <vector>
#include <string>

#include "resource.h"

struct MonitorInfo {
    std::wstring id;
    std::wstring path;
};

INT_PTR CALLBACK DialogProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    static std::vector<MonitorInfo>* monitors = nullptr;

    switch (message) {
    case WM_INITDIALOG:
        monitors =
            reinterpret_cast<std::vector<MonitorInfo>*>(lParam);

        for (size_t i = 0; i < monitors->size(); ++i) {
            std::wstring label =
                L"Monitor " + std::to_wstring(i + 1);

            SendDlgItemMessageW(
                hwnd,
                IDC_MONITOR_LIST,
                LB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(label.c_str())
            );
        }

        SendDlgItemMessageW(
            hwnd,
            IDC_MONITOR_LIST,
            LB_SETCURSEL,
            0,
            0
        );

        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            LRESULT selected = SendDlgItemMessageW(
                hwnd,
                IDC_MONITOR_LIST,
                LB_GETCURSEL,
                0,
                0
            );

            if (selected != LB_ERR) {
                EndDialog(hwnd, selected);
            }

            return TRUE;
        }

        case IDCANCEL:
            EndDialog(hwnd, -1);
            return TRUE;
        }

        break;
    }

    return FALSE;
};

int main() {
    HRESULT hr = CoInitialize(nullptr);

    if (FAILED(hr)) {
        std::cerr << "CoInitialize failed\n";
        return -1;
    }

    IDesktopWallpaper* wallpaper = nullptr;

    hr = CoCreateInstance(
        CLSID_DesktopWallpaper,
        nullptr,
        CLSCTX_ALL,
        IID_PPV_ARGS(&wallpaper)
    );

    if (FAILED(hr)) {
        std::cerr << "Failed to create DesktopWallpaper COM object\n";
        CoUninitialize();
        return -1;
    }

    UINT count = 0;
    hr = wallpaper->GetMonitorDevicePathCount(&count);

    if (FAILED(hr)) {
        std::cerr << "Failed to get display count\n";
        CoUninitialize();
        wallpaper->Release();
        return -1;
    }

    if (count <= 0) {
        std::cerr << "Error, no monitors detected.\n";
        CoUninitialize();
        wallpaper->Release();
        return -1;
    }

    std::vector<MonitorInfo> monitors;

    // populate monitors vector with corresponding id and path
    // that way when we create the dialog box there are options to choose from
    for (UINT i = 0; i < count; ++i) {
        LPWSTR monitorId = nullptr;
        // if there are valid displays / on success
        if (SUCCEEDED(
                wallpaper->GetMonitorDevicePathAt(i, &monitorId))) {
            RECT rect;

            if (SUCCEEDED(
                    wallpaper->GetMonitorRECT(monitorId, &rect))) {
                LPWSTR path = nullptr;
                if (SUCCEEDED(wallpaper->GetWallpaper(monitorId, &path))) {
                    monitors.push_back({monitorId, path});
                }  
                CoTaskMemFree(path);
            }

        }
        CoTaskMemFree(monitorId);
    }
    // if only one display, open file explorer with file selected directly
    // if more, open dialog box allowing for wallpaper selection
    if (monitors.size() == 1) {        
        
        std::wstring args = L"/select,\"";
        args += monitors[0].path;
        args += L"\"";

        HINSTANCE result = ShellExecuteW(
                nullptr,
                L"open",
                L"explorer.exe",
                args.c_str(),
                nullptr,
                SW_SHOWNORMAL
        );

        if ((INT_PTR)result < 32) {
            std::cerr << "Shell Execute Failure\n";
            CoUninitialize();
            wallpaper->Release();
            return -1;
        }
    }
    else { // multiple monitors
        HINSTANCE hInstance = GetModuleHandleW(nullptr);
        INT_PTR selected = DialogBoxParamW(
            hInstance,
            MAKEINTRESOURCEW(IDD_MONITOR_DIALOG),
            nullptr,
            DialogProc,
            reinterpret_cast<LPARAM>(&monitors)
        );
        if (selected == -1) {
            DWORD error = GetLastError();

            std::cerr << "DialogBoxParamW failed. Error: "
                    << error << '\n';

            wallpaper->Release();
            CoUninitialize();
            return -1;
        }
        else if (selected >= 0) {
            std::wstring args = L"/select,\"";
            args += monitors[selected].path;
            args += L"\"";

            HINSTANCE result = ShellExecuteW(
                    nullptr,
                    L"open",
                    L"explorer.exe",
                    args.c_str(),
                    nullptr,
                    SW_SHOWNORMAL
            );

            if ((INT_PTR)result < 32) {
                std::cerr << "Shell Execute Failure\n";
                CoUninitialize();
                wallpaper->Release();
                return -1;
            }
        }
    }

    wallpaper->Release();
    CoUninitialize();

    return 0;
}