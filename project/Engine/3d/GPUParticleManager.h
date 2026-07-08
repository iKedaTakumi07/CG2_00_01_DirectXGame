#pragma once
#include <d3d12.h>
#include <list>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <wrl.h>
#include "../base/Math.h"

class DirectXCommon;
class SrvManager;
class Camera;
class WinApp;

class GPUParticleManager {
public:
    // コンストラクタに渡すための鍵
    class ConstructorKey {
    private:
        ConstructorKey() = default;
        friend class GPUParticleManager;
    };

    // passkeyを受け取るコンストラクタ
    explicit GPUParticleManager(ConstructorKey) { }

    static GPUParticleManager* getInstance();

    // 初期化
    void Initialize(DirectXCommon* DirectXCollision, SrvManager* srvManager);

    void Update();

    void Draw();

    // set
    void SetDefaultCamera(Camera* camera) { this->Camera_ = camera; }

    // CS
    void PrepareCSObjectDraw();
    void CSInitialize();

    GPUParticleManager(GPUParticleManager&) = delete;
    GPUParticleManager& operator=(GPUParticleManager&) = delete;

private:
    friend struct std::default_delete<GPUParticleManager>;
    ~GPUParticleManager() = default;

private:
    void RootSignatureInitialize(DirectXCommon* dxcommon);
    void graphicsPipelineInitialize(DirectXCommon* dxcommon);

    // Cs用パイプライン
    void CSRootSignatureInitialize(DirectXCommon* dxcommon);
    void CSPipelineInitialize(DirectXCommon* dxcommon);

    void ResourceInitialize();

private:
    SrvManager* srvManager = nullptr;
    DirectXCommon* dxCommon = nullptr;
    WinApp* winApp_ = nullptr;

    Camera* Camera_ = nullptr;

    // 各リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
    Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource;
    uint32_t instancingSrvIndex = 0;
    uint32_t textureSrvIndex = 0;

    // IA
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView { };
    VertexData* vertexData = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> csRootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> csPipelineState;
    Microsoft::WRL::ComPtr<ID3D12Resource> particleResource_;

    static std::unique_ptr<GPUParticleManager> instance;
    const int kMaxParticles = 1024;
};
