#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <iostream>


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
    count = GetSystemMetrics(SM_CMONITORS);

    if (count <= 0) {
        std::cerr << "Error, no monitors detected.\n";
        CoUninitialize();
        wallpaper->Release();
        return -1;
    }
    else if (count == 1) {
        LPWSTR monitorId = nullptr;
        if (FAILED(wallpaper->GetMonitorDevicePathAt(1, &monitorId))) {
            std::cerr << "Failed to create monitorid object\n";
            CoUninitialize();
            wallpaper->Release();
            return -1;
        }

        RECT rect;

        if (FAILED(wallpaper->GetMonitorRECT(monitorId, &rect))) {
            std::cerr << "Failed to create rect object\n";
            CoUninitialize();
            CoTaskMemFree(monitorId);
            wallpaper->Release();
            return -1;
        }

        LPWSTR path = nullptr;

        if (FAILED(wallpaper->GetWallpaper(monitorId, &path))) {
            std::cerr << "Failed to create path object\n";
            CoUninitialize();
            CoTaskMemFree(monitorId);
            wallpaper->Release();
            return -1;
        }
        
        
        std::wstring args = L"/select,\"";
        args += path;
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
            CoTaskMemFree(monitorId);
            CoTaskMemFree(path);
            wallpaper->Release();
            return -1;
        }
        
        CoTaskMemFree(monitorId);
        CoTaskMemFree(path);
    }
    else {
        for (UINT i = 0; i < count; ++i) {
            LPWSTR monitorId = nullptr;
            // if there are valid displays / on success
            if (SUCCEEDED(
                    wallpaper->GetMonitorDevicePathAt(i, &monitorId))) {
                RECT rect;

                if (SUCCEEDED(
                        wallpaper->GetMonitorRECT(monitorId, &rect))) {
                    LPWSTR path = nullptr;


                    if (SUCCEEDED(
                        wallpaper->GetWallpaper(monitorId, &path))) {

                    std::wcout << L"Monitor " << i << L":\n";
                    std::wcout << L"  " << path << L"\n";
                    }
                    CoTaskMemFree(path);
                }

            }
            CoTaskMemFree(monitorId);
        }
    }

    wallpaper->Release();
    CoUninitialize();

    return 0;
}