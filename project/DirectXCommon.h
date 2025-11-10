#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXCommon {
public:
    // 初期化
    void Initialize(WinApp* winApp);

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

    void DepthBufferInitialize();

    void DescriptorInitialize();
    uint32_t desriptorSizeSRV;
    uint32_t desriptorSizeRTV;
    uint32_t desriptorSizeDSV;
};
