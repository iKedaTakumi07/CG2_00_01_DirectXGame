#pragma once
#include "WinApp.h"
#include <array>
#include <cassert>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXCommon {
public:
    // 初期化
    void Initialize(WinApp* winApp);

    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

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
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescripotrHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

    void rtvInitialize();
    // SwapChainResource
    /*Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];*/
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

    // RTVを二つ作るのでディスクリプタを2用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

    void DepthStencilInitialize();

};