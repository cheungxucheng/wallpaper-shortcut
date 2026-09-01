#include <windows.h>
#include <shobjidl.h>
#include <iostream>

int main() {
    HRESULT hr = CoInitialize(nullptr);

    if (FAILED(hr)) {
        std::cerr << "CoInitialize failed\n";
        return 1;
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
        return 1;
    }

    UINT count = 0;
    hr = wallpaper->GetMonitorDevicePathCount(&count);

    // look into getmonitorrect for active displays
    // 
    if (SUCCEEDED(hr)) {
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