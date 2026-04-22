#pragma once
#include <cstdint>
class DirectXCommon;
class SrvManager;

class OffscreenSuface {
public:
    // 初期化
    void Initialize(DirectXCommon* dxcommon, SrvManager* srvManager, uint32_t width, uint32_t height);

    // 描画先をこのテクスチャに切り替える
    void PreDraw();
    // 描画を終了し、テクスチャとして使える状態にする
    void PostDraw();

    // ハンドル取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() { return srvHandleGPU; }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    uint32_t rtvIndex;
    uint32_t srvIndex;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
};
