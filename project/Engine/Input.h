#pragma once
#include <Windows.h>

class Input {
public:
    // 初期化
    void Initialize(HWND hwnd, HINSTANCE hInstance);
    // 更新
    void Update();
};
