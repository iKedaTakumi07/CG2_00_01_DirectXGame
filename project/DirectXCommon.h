#pragma once
#include "externals/DirectXTex/d3dx12.h"

#include "WinApp.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <array>
#include <cassert>
#include <d3d12.h>
#include <dxcapi.h>
#include <chrono>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXCommon {
public:
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

    // シェーダーのコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);
    // バッフアリソースの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizwInBytes);
    // テクスチャリソースの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
    // テクスチャデータの転送
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
   

    // 初期化
    void Initialize(WinApp* winApp);

    void PreDraw();

    void PostDraw();
    void FlushCommandQueue();

    // getter
    ID3D12Device* GetDevice() const { return device.Get(); };
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
    HANDLE GetfenceEvent() { return fenceEvent; }

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
    
    // 最大SRV
    static const uint32_t kMaxSRVCount;

private:
    // WindousAPI
    WinApp* winApp_ = nullptr;

    void deviceInitialize();
    // DirectX12デバイス
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    // DXGIファクトリ-
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

    void CommonInitialize();
    // コマンドキュー
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    // コマンドリストを生成する
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    // コマンドアロケータを生成する
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

    void swapChainInitialize();
    // スワップチェーンを生成する
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc {};

    void DepthBufferInitialize();
    // Resourceの設定
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;

    void DescriptorInitialize();
    uint32_t desriptorSizeSRV;
    uint32_t desriptorSizeRTV;
    uint32_t desriptorSizeDSV;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescripotrHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

    void rtvInitialize();
    // SwapChainResource
    /*Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];*/
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};

    // RTVを二つ作るのでディスクリプタを2用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

    void DepthStencilInitialize();

    void fenceInitialize();
    // 初期値0でfenecを作る
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    uint64_t fenceValue = 0;
    HANDLE fenceEvent;

    void viewportInitialize();
    // びゅーポート
    D3D12_VIEWPORT viewport {};

    void scissorRectInitialize();
    // シザー矩形
    D3D12_RECT scissorRect {};

    void dxcCompilerInitialize();
    // dxcCompilerを初期化
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

    void ImguiInitialize();

    // FPS固定初期化
    void InitializeFixFPS();
    void UpdateFixFPS();
    // 記録時間FPS固定用
    std::chrono::steady_clock::time_point reference_;


};