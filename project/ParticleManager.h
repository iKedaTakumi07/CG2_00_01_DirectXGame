#pragma once
#include <d3d12.h>
#include <list>
#include <random>
#include <string>
#include <unordered_map>
#include <wrl.h>

#include "Math.h"

class DirectXCommon;
class SrvManager;

class ParticleManager {
public:
    // struct ParticleGroup {
    //     MaterialData* materialData; // マテリアルデータ
    //     std::list<Particle> Particles; // パーティクルリスト
    //     D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc; // SRVインデクス
    //     Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource; // インスタンシングデータ
    //     const uint32_t kNumMaxInstance = 100; // インスタンス数
    //     ParticleForGPU* instancingData; // インスタンシングデータ
    // };

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
    };

    static ParticleManager* getInstance();

    void Finalize();

    // 初期化
    void Initialize(DirectXCommon* DirectXCollision, SrvManager* srvManager);

    void Update();

    void CreateParticleGroup(const std::string name, const std::string textureFilePath);

private:
    SrvManager* srvManager = nullptr;
    DirectXCommon* dxCommon;

    ModelData model;

    static ParticleManager* instance;

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
    VertexData* VertexData = nullptr;

    std::unordered_map<std::string, ParticleGroup> particleGroups;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

    std::mt19937 randomEngine;
};
