#pragma once
#include <random>
#include <d3d12.h>
#include "Math.h"
#include <list>

class DirectXCommon;
class SrvManager;
class Model;

class ParticleManager {
public:
    struct ParticleGroup {
        Material* materialData; // マテリアルデータ
        std::list<Particle> Particles; // パーティクルリスト
        D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc; // SRVインデクス
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource; // インスタンシングデータ
        const uint32_t kNumMaxInstance = 100; // インスタンス数
        ParticleForGPU* instancingData; // インスタンシングデータ
    };

    static ParticleManager* getInstance();

    void Finalize();

    // 初期化
    void Initialize(DirectXCommon* DirectXCollision, SrvManager* srvManager);

    void CreateParticleGroup(const std::string name, const std::string textureFilePath);

private:
    SrvManager* srvManager = nullptr;
    DirectXCommon* dxCommon;
    Model* model = nullptr;

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
    // std::random_device seedGenerator;
    // std::mt19937 randomEngine;
};
