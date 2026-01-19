#pragma once
#include "Math.h"
#include <d3d12.h>
#include <list>
#include <random>
#include <string>
#include <unordered_map>
#include <wrl.h>

class DirectXCommon;
class SrvManager;
class Camera;
class WinApp;

class ParticleManager {
public:
    struct ParticleGroup {
        // 1. マテリアルデータ
        MaterialData material;
        // 2. パーティクルリスト
        std::list<Particle> particles;
        // 3. テクスチャ用 SRV インデックス
        uint32_t textureSrvIndex = 0;
        // 4. インスタンシング用リソース
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
        // 5. インスタンス数
        uint32_t instanceCount = 0;
        // 6. インスタンシング用 SRV インデックス
        uint32_t instancingSrvIndex = 0;
        // マテリアルリソース
        Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
    };

    static ParticleManager* getInstance();

    void Finalize();

    // 初期化
    void Initialize(DirectXCommon* DirectXCollision, SrvManager* srvManager/*, WinApp* winApp*/);

    void Update();

    void Draw();

    void CreateParticleGroup(const std::string name, const std::string textureFilePath);

    void Emit(const std::string name, const Vector3& position, uint32_t count);

    // set
    void SetDefaultCamera(Camera* camera) { this->Camera_ = camera; }

private:
    SrvManager* srvManager = nullptr;
    DirectXCommon* dxCommon;
    WinApp* winApp_ = nullptr;

    Camera* Camera_ = nullptr;

    ModelData model;

    static ParticleManager* instance;
    const uint32_t kMaxInstanceCount = 100;
    const float kDeltaTime = 1.0f / 60.0f;

    bool useBillboard = false;

    ParticleManager() = default;
    ~ParticleManager() = default;
    ParticleManager(ParticleManager&) = delete;
    ParticleManager& operator=(ParticleManager&) = delete;

    void RootSignatureInitialize(DirectXCommon* dxcommon);
    void graphicsPipelineInitialize(DirectXCommon* dxcommon);
    void VertexResourceInitialize();
    void MaterialResourceInitialize();

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    VertexData* vertexData = nullptr;

    std::unordered_map<std::string, ParticleGroup> particleGroups;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

    std::mt19937 randomEngine;
};
