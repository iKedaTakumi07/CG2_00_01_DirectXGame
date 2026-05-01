#include "OffscreenSurface.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "WinApp.h"

void OffscreenSurface::Initialize(DirectXCommon* dxcommon, SrvManager* srvManager, uint32_t rtvIndex)
{
    dxCommon_ = dxcommon;
    srvManager_ = srvManager;
    this->rtvIndex = rtvIndex;

    // テクスチャリソース作成
    Vector4 clearColor = { 1.0f, 0.0f, 0.0f, 1.0f }; // わかりやすいようにいったん赤
    resource = dxcommon->CreateRenderTextureResource(WinApp::KClientWidth, WinApp::KClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);

    // RTVの作成
    rtvHandleCPU = dxcommon->GetRTVCPUDescriptorHandle(rtvIndex);

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    dxCommon_->GetDevice()->CreateRenderTargetView(resource.Get(), &rtvDesc, rtvHandleCPU);

    // SRVの作成
    srvIndex = srvManager_->Allocate();
    srvHandleGPU = srvManager_->GetGPUDescriptorHandle(srvIndex);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    dxCommon_->GetDevice()->CreateShaderResourceView(resource.Get(), &srvDesc, srvManager_->GetCPUDescriptorHandle(srvIndex));

    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    D3D12_RESOURCE_BARRIER barrier {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource.Get();
    // 作成直後は RENDER_TARGET なので、それを PIXEL_SHADER_RESOURCE に変えておく
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    commandList->ResourceBarrier(1, &barrier);

    // コマンドを確定させて実行し、完了を待つ（これでステートが PSR で確定する）
    dxCommon_->FlushCommandQueue();
}

void OffscreenSurface::PreDraw()
{
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // バリア
    D3D12_RESOURCE_BARRIER barrier {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = resource.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandList->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxCommon_->GetDSVCPUDescriptorHandle();
    commandList->OMSetRenderTargets(1, &rtvHandleCPU, false, &dsvHandle);
    float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // わかりやすいようにいったん赤

    // 視座―
    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)WinApp::KClientWidth, (float)WinApp::KClientHeight, 0.0f, 1.0f };
    D3D12_RECT scissorRect = { 0, 0, WinApp::KClientWidth, WinApp::KClientHeight };

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    commandList->ClearRenderTargetView(rtvHandleCPU, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void OffscreenSurface::PostDraw()
{
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // バリア
    D3D12_RESOURCE_BARRIER barrier {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = resource.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    commandList->ResourceBarrier(1, &barrier);
}
