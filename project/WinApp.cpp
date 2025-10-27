#include "WinApp.h"
void WinApp::Initialize()
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    WNDCLASS wc {};
    // ウィンドウプロ―ジャ
    wc.lpfnWndProc = Windowproc;
    // ウィンドウクラス名(なんでもよし)
    wc.lpszClassName = L"CG2WindowClass";
    // インスタンスハンドル
    wc.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    // ウィンドウクラスを登録する
    RegisterClass(&wc);

    // クライアント領域のサイズ
    const int32_t KClientWidth = 1280;
    const int32_t KClientHeight = 720;

    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0, 0, KClientWidth, KClientHeight };

    // クライアント領域を元に実際のサイズをwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
    // ウィンドウの作成
    HWND hwnd = CreateWindow(
        wc.lpszClassName, // 利用するクラス名
        L"CG2", // タイトルバーの文字
        WS_OVERLAPPEDWINDOW, // よく見るウィンドウスタイル
        CW_USEDEFAULT, // 座標X表示
        CW_USEDEFAULT, // 座標Y表示
        wrc.right - wrc.left, // ウィンドウ横幅
        wrc.bottom - wrc.top, // ウィンドウ縦幅
        nullptr, // メニュースタイル
        nullptr, wc.hInstance, // インスタンスハンドル
        nullptr); // オプション

    // ウィンドウを表示する
    ShowWindow(hwnd, SW_SHOW);
}

void WinApp::Update()
{
}