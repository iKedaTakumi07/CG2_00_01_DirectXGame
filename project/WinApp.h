#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp {
public: // 静的メンバ関数
    static LRESULT CALLBACK Windowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
    // 初期化
    void Initialize();
    // 更新
    void Update();

    // Getter
    HWND GetHwnd() const { return hwnd; }

    // getter
    HINSTANCE GetHInstance() const { return wc.hInstance; };

public:
    // クライアント領域のサイズ
    static const int32_t KClientWidth = 1280;
    static const int32_t KClientHeight = 720;

private:
    HWND hwnd = nullptr;

    WNDCLASS wc {};
};
