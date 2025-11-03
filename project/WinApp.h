#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp {
public:
    static LRESULT CALLBACK Windowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
    // 初期化
    void Initialize();

    // 更新
    void Update();

    // delete
    void Finalize();


    // getter
    HWND GetHwnd() const { return hwnd; }
    HINSTANCE GetHInstance() const { return wc.hInstance; }

public:
    // クライアント領域のサイズ
    static const int32_t KClientWidth = 1280;
    static const int32_t KClientHeight = 720;

private:
    // ウィンドウハンドル
    HWND hwnd = nullptr;
    // ウィンドウクラスの設定
    WNDCLASS wc {};
};
