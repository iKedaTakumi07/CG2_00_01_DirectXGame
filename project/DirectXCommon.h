#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXCommon {
public:
    // 初期化
    void Initialize(WinApp* winApp);

    void CommonInitialize();

    DXGI_SWAP_CHAIN_DESC1 swapChainInitialize();

    Microsoft::WRL::ComPtr<ID3D12Resource> DepthBufferInitialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, int32_t width, int32_t height);

private:
    // DirectX12デバイス
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    // DXGIファクトリ
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
    // WindousAPI
    WinApp* winApp_ = nullptr;
};
