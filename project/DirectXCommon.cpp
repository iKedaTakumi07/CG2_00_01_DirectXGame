#include "DirectXCommon.h"
#include "Logger.h"
#include "StringUtility.h"

#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include <filesystem>
#include <format>
#include <fstream>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVCPUDescriptorHandle(uint32_t index)
{
    return GetCPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVGPUDescriptorHandle(uint32_t index)
{
    return GetGPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, index);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescripotrHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc {};
    DescriptorHeapDesc.Type = heapType; // レンダーターゲットビュー用
    DescriptorHeapDesc.NumDescriptors = numDescriptors; // ダブルバッファ用に2つ。多くてもかまわない
    DescriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescripotrHeap));

    // ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));
    return DescripotrHeap;
};

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

void DirectXCommon::Initialize(WinApp* winApp)
{
    // null検出
    assert(winApp);

    this->winApp_ = winApp;

    deviceInitialize(); // デバイス
    CommonInitialize(); // コマンド関連
    swapChainInitialize(); // スワップチェーン
    DepthBufferInitialize(); // 深度バッフア
    DescriptorInitialize(); // 各種デスクリプタヒープ
    rtvInitialize(); // レンダーターゲットビュー
    DepthStencilInitialize(); // 深度ステンシル
    fenceInitialize(); // フェンス
    viewportInitialize(); // ビューポート
    scissorRectInitialize(); // シザリング矩形
    dxcCompilerInitialize(); // DXCコンパイラ
    ImguiInitialize(); // Imgui
}

void DirectXCommon::PreDraw()
{
    // バックバッファのインデックス取得
    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    // TransitionBarrierの設定
    D3D12_RESOURCE_BARRIER barrier {};
    // 今回のバリアはTransutuion
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    // noneにしておく
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    // バリアを貼る対象のリソース。現在のバックバッファに対して行う
    barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
    // 繊維前のResourceState
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    // 遷移後のResourceState
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    // TransitionBarrierを張る
    commandList->ResourceBarrier(1, &barrier);

    // 描画先のRTVを設定する
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
    // 指定した色で画面全体をクリアにする
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; // 青っぽい色。RGBAの順番
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

    // 描画用のDescriptorHeapの設定
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHraps[] = { srvDescriptorHeap.Get() };
    commandList->SetDescriptorHeaps(1, descriptorHraps->GetAddressOf());

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
}

void DirectXCommon::PostDraw()
{
    HRESULT hr;

    // バックバッファのインデックス取得
    UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    // TransitionBarrierの設定
    D3D12_RESOURCE_BARRIER barrier {};

    // 画面に各処理は全て終わり、画面に移すので、状態を遷移

    // 今回のバリアはTransutuion
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    // noneにしておく
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    // バリアを貼る対象のリソース。現在のバックバッファに対して行う
    barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();

    // 今回hRenderTargetからPresentにする
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    // TransitionBarrierを張る
    commandList->ResourceBarrier(1, &barrier);

    // 描画先のRTVを設定する
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    // 指定した震度で画面全体をクリアする
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // コマンドリストの内容を確定させる
    hr = commandList->Close();
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList };
    commandQueue->ExecuteCommandLists(1, commandLists->GetAddressOf());
    // GPUとOSに画面交換を行うように通知する
    swapChain->Present(1, 0);

    // Fenceの値を更新
    fenceValue++;
    // GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence.Get(), fenceValue);

    // Fenceの値が指定したSignal値にたどり着いているか確認する
    // GetcompletedValueの初期値はFence作成時に渡した初期値
    if (fence->GetCompletedValue() < fenceValue) {
        // 指定したSignalにたどり着いてないので、たどり着くまで待つようにイベントを設定する
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        // イベントを待つ
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    // 次のフレーム用のコマンドリストを準備
    hr = commandAllocator->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList->Reset(commandAllocator.Get(), nullptr);
    assert(SUCCEEDED(hr));
}

void DirectXCommon::deviceInitialize()
{
    HRESULT hr;

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバックレイヤーを有効化
        debugController->EnableDebugLayer();
        // さらにGPU側でもチェックを行う
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif // _DEBUG

    // ログのディレクトリを用意
    std::filesystem::create_directory("logs");
    // 現在時刻を取得(UTC)
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    // ログファイルの名前にコンマ何秒はいらぬから削る
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    // 日本時間(pcの設定)に変更
    std::chrono::zoned_time localTime { std::chrono::current_zone(), nowSeconds };
    // formatを使って年月日_時分秒に変換
    std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
    // 時刻を使ってファイル名を決定
    std::string logFilePath = std::string("logs/") + dateString + ".log";
    // ファイルを作って書き込み準備
    std::ofstream logStream(logFilePath);

    // 出力ウィンドウへの文字出力
    Logger::Log(logStream, "Hello,DirectX!\n");
    Logger::Log(logStream, StringUtility::ConvertString(std::format(L"clientSize{},{}\n", WinApp::KClientWidth, WinApp::KClientHeight)));

    // 関数が成功したかマクロ判定
    hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
    // assertにしておく
    assert(SUCCEEDED(hr));

    // 使用するアダプタ用の変数
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプターの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc {};
        hr = useAdapter->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr)); // 取得できないのは一大事
        // ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // ログに出力
            Logger::Log(logStream, StringUtility::ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
            break;
        }
        useAdapter = nullptr;
    }
    // 適切なアダプタが見つからなかったので起動できない
    assert(useAdapter != nullptr);

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
        hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
        // 　指定したレベルで生成できたか確認
        if (SUCCEEDED(hr)) {
            Logger::Log(logStream, std::format("FeatureLevel :{}\n", featureLevelStrings[i]));
            break;
        }
    }

    // デバイスの生成がうまくいかなかったので起動できない
    assert(device != nullptr);
    Logger::Log(logStream, "Complete create D3D12Device!\n"); // 初期化完了のログを出す
#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // やばいエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラー時に泊まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警報時に泊まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // 制御するメッセージのID
        D3D12_MESSAGE_ID denyids[] = {
            // windows11でのDXGIデバックレイヤーとDX12デバックれいやーの相互作用バグによるエラーメッセージ
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter {};
        filter.DenyList.NumIDs = _countof(denyids);
        filter.DenyList.pIDList = denyids;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 措定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);
    }

#endif // _DEBUG
}

void DirectXCommon::CommonInitialize()
{
    HRESULT hr;

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc {};
    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
    // コマンドキューの生成がうまくいかなかったから起動できない
    assert(SUCCEEDED(hr));

    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    // コマンドアロケータの生成がうまくいかなかったから起動できない
    assert(SUCCEEDED(hr));

    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    // コマンドリストの生成がうまくいかなかったから起動できない
    assert(SUCCEEDED(hr));
}

void DirectXCommon::swapChainInitialize()
{
    HRESULT hr;

    swapChainDesc.Width = winApp_->KClientWidth; // 画面の幅
    swapChainDesc.Height = winApp_->KClientHeight; // 画面の高さ
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
    swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 画面のターゲットとして利用する
    swapChainDesc.BufferCount = 2; // ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニターに移したら、中身を破棄

    // コマンドキュー、ウィンドウハンドル、設定をして渡す
    hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
}

void DirectXCommon::DepthBufferInitialize()
{

    D3D12_RESOURCE_DESC resourceDesc {};
    resourceDesc.Width = winApp_->KClientWidth;
    resourceDesc.Height = winApp_->KClientHeight;
    resourceDesc.MipLevels = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // 利用するheaoの設定
    D3D12_HEAP_PROPERTIES heapProperties {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 震度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue {};
    depthClearValue.DepthStencil.Depth = 1.0f;
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue, IID_PPV_ARGS(&resource));

    assert(SUCCEEDED(hr));
}

void DirectXCommon::DescriptorInitialize()
{
    // descriptorSize
    desriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    desriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    desriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    // RTVデスクリプタヒープの生成
    rtvDescripotrHeap = createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    // SRV用のヒープでディスクリプタの数128。SRVはShadre内で触るものなので、ShaderVisiblrはtrue
    srvDescriptorHeap = createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
    // DSV用のひーぷでディスクリプタの数は1
    dsvDescriptorHeap = createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

void DirectXCommon::rtvInitialize()
{
    HRESULT hr;

    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    // うまく起動できなければ起動できない
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));

    // RTVの設定

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む
    // 　ディスクリプタの先頭を取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescripotrHeap->GetCPUDescriptorHandleForHeapStart();

    // まず1つ目
    rtvHandles[0] = rtvStartHandle;
    device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
    // 2つ目作成
    rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
}

void DirectXCommon::DepthStencilInitialize()
{
    // DSV
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    // DSVheapの先頭にDSVを作る
    device->CreateDepthStencilView(resource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::fenceInitialize()
{
    HRESULT hr;

    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(hr));

    // FenceのSignalを待つためのイベントを作る
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);
}

void DirectXCommon::viewportInitialize()
{

    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = winApp_->KClientWidth;
    viewport.Height = winApp_->KClientHeight;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
}

void DirectXCommon::scissorRectInitialize()
{

    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = winApp_->KClientWidth;
    scissorRect.top = 0;
    scissorRect.bottom = winApp_->KClientHeight;
}

void DirectXCommon::dxcCompilerInitialize()
{
    HRESULT hr;

    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    // 現時点ではincludeはしないが、nicludeに対するための設定を行っておく
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));
}

void DirectXCommon::ImguiInitialize()
{
    // ImGui初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(winApp_->GetHwnd());
    ImGui_ImplDX12_Init(device.Get(),
        swapChainDesc.BufferCount,
        rtvDesc.Format,
        srvDescriptorHeap.Get(),
        srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}
