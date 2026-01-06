#include "ImGuiManager.h"
#include "WinApp.h"

#include "DirectXCommon.h"
#include "SrvManager.h"

void ImGuiManager::Initialize([[maybe_unused]] WinApp* winApp, [[maybe_unused]] DirectXCommon* dxCommon, [[maybe_unused]] SrvManager* srvManager)
{
#ifdef USE_IMGUI

    winApp_ = winApp;
    dxCommon_ = dxCommon;
    srvManager_ = srvManager;

    // imguiのコンテキスト生成
    ImGui::CreateContext();

    // imguiのスタイルを設定
    ImGui::StyleColorsDark();

    // win32初期化
    ImGui_ImplWin32_Init(winApp_->GetHwnd());

    // srv取得
    SrvIndex = srvManager_->Allocate();

    // directX12初期化
    IMGUI_CHECKVERSION();
    ImGui_ImplDX12_Init(
        dxCommon_->GetDevice(),
        static_cast<int>(dxCommon_->GetSwapChainResourcesNum()),
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        srvManager_->GetdescriptorHeap().Get(),
        srvManager_->GetCPUDescriptorHandle(SrvIndex),
        srvManager_->GetGPUDescriptorHandle(SrvIndex));
#endif // USE_IMGUI
}

void ImGuiManager::Begin()
{
#ifdef USE_IMGUI
    // ImGuiフレーム開始
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif // USE_IMGUI
}

void ImGuiManager::End()
{
#ifdef USE_IMGUI
    // 描画前準備
    ImGui::Render();
#endif // USE_IMGUI
}

void ImGuiManager::Draw()
{
#ifdef USE_IMGUI
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // デスクリプタヒープの配列をセットコマンドするコマンド
    ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetdescriptorHeap().Get() };
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    // 描画コマンド
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif // USE_IMGUI
}

void ImGuiManager::Finalize()
{
#ifdef USE_IMGUI
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif // USE_IMGUI
}