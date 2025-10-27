#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h>
#include <wrl.h>
#include <dinput.h>

class Input {
public:
    // namespace省略
    template <class T> using ComPrt = Microsoft::WRL::ComPtr<T>;

public:
    // 初期化
    void Initialize(HWND hwnd, HINSTANCE hInstance);
    // 更新
    void Update();

private:
    ComPrt<IDirectInputDevice8> keyboard;
};
