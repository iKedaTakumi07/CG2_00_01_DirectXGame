#include <Windows.h>
#include <cstdint>
#include <format>
#include <string>

// string->wstring
std::wstring ConvertString(const std::string& str)
{
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

// wstring->string
std::string ConvertString(const std::wstring& str)
{
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

void Log(const std::string& message)
{
    OutputDebugStringA(message.c_str());
}

// ウィンドウプロ―ジャ
LRESULT CALLBACK Windowproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破壊された
    case WM_DESTROY:
        // OSに対してアプリの終了を伝える
        PostQuitMessage(0);
        return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
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
    // 出力ウィンドウへの文字出力
    Log("Hello,DirectX!\n");
    Log(ConvertString(std::format(L"clientSize{},{}\n", KClientWidth, KClientHeight)));

    MSG msg {};
    // ウィンドウの×ボタンが押されるまでループ
    while (msg.message != WM_QUIT) {
        // windowにメッセージが来ていたら最優先で処理させる
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            // ゲームの処理
        }
    }

    // 変数から型を推論してくれる
    Log(std::format("enemyHP:{},TexturePath:{}\n", 10, 1));

    return 0;
}
