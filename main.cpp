#include <Windows.h>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <d3d12.h>
#include <dbghelp.h>
#include <dxgi1_6.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <strsafe.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")

// CrashHandler
static LONG WINAPI ExportDump(EXCEPTION_POINTERS* excption)
{
    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t filePath[MAX_PATH] = { 0 };
    CreateDirectory(L"./Dumps", nullptr);
    StringCchPrintf(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
    HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    // processId とクラッシュの発生したthreadidを取得
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();
    // 設定情報を入力
    MINIDUMP_EXCEPTION_INFORMATION minidumpInformation { 0 };
    minidumpInformation.ThreadId = threadId;
    minidumpInformation.ExceptionPointers = excption;
    minidumpInformation.ClientPointers = TRUE;

    // Dumpを出力
    MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);

    return EXCEPTION_EXECUTE_HANDLER;
}

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

void Log(std::ostream& os, const std::string& message)
{
    os << message << std::endl;
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

    // 誰も捕捉しなかった場合に、捕捉する関数を登録
    SetUnhandledExceptionFilter(ExportDump);

    // ログのディレクトリを用意
    std::filesystem::create_directory("logs");
    // 現在時刻を取得(UTC)
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    // ログファイルの名前にコンマ何秒はいらぬから削る
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
        nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    // 日本時間(pcの設定)に変更
    std::chrono::zoned_time localTime { std::chrono::current_zone(), nowSeconds };
    // formatを使って年月日_時分秒に変換
    std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
    // 時刻を使ってファイル名を決定
    std::string logFilePath = std::string("logs/") + dateString + ".log";
    // ファイルを作って書き込み準備
    std::ofstream logStream(logFilePath);

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
    Log(logStream, "Hello,DirectX!\n");
    Log(logStream, ConvertString(std::format(L"clientSize{},{}\n", KClientWidth, KClientHeight)));

    // DXGIファクトリーの生成
    IDXGIFactory7* dxgiFactory = nullptr;
    // 関数が成功したかマクロ判定
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    // assertにしておく
    assert(SUCCEEDED(hr));

    // 使用するアダプタ用の変数
    IDXGIAdapter4* useAdapter = nullptr;
    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプターの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc {};
        hr = useAdapter->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr)); // 取得できないのは一大事
        // ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // ログに出力
            Log(logStream, ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
            break;
        }
        useAdapter = nullptr;
    }
    // 適切なアダプタが見つからなかったので起動できない
    assert(useAdapter != nullptr);

    ID3D12Device* device = nullptr;
    // 昨日レベルログ出力に文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
    // 高い順に生成する
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 指定したあだぷたーでデバイスを作成
        hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
        // 　指定したレベルで生成できたか確認
        if (SUCCEEDED(hr)) {
            Log(logStream, std::format("FeatureLevel :{}\n", featureLevelStrings[i]));
            break;
        }
    }
    // デバイスの生成がうまくいかなかったので起動できない
    assert(device != nullptr);
    Log(logStream, "Complete create D3D12Device!\n"); // 初期化完了のログを出す

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

    return 0;
}
