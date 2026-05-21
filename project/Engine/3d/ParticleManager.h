#pragma once
#include "../base/Math.h"
#include <d3d12.h>
#include <list>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <wrl.h>

#include "IParticleMesh.h"
#include <numbers>

class DirectXCommon;
class SrvManager;
class Camera;
class WinApp;

// パーティクルのタイプ
enum class ParticleMeshType {
    Plane,
    Ring
};

struct EmitterParam {
    // スケールの最小・最大
    Vector3 minScale = { 1.0f, 1.0f, 1.0f };
    Vector3 maxScale = { 1.0f, 1.0f, 1.0f };

    // 回転の最小・最大 (ラジアン)
    Vector3 minRotate = { 0.0f, 0.0f, -std::numbers::pi_v<float> };
    Vector3 maxRotate = { 0.0f, 0.0f, std::numbers::pi_v<float> };

    // 速度の最小・最大
    Vector3 minVelocity = { -1.0f, -1.0f, -1.0f };
    Vector3 maxVelocity = { 1.0f, 1.0f, 1.0f };

    // 色の最小・最大
    Vector4 minColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4 maxColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 寿命の最小・最大
    float minLifeTime = 1.0f;
    float maxLifeTime = 2.0f;
};

class ParticleManager {
public:
    // コンストラクタに渡すための鍵
    class ConstructorKey {
    private:
        ConstructorKey() = default;
        friend class ParticleManager;
    };

    // passkeyを受け取るコンストラクタ
    explicit ParticleManager(ConstructorKey) { }

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
        // グループごとのメッシュ
        std::unique_ptr<IParticleMesh> mesh;
    };

    static ParticleManager* getInstance();

    void Finalize();

    // 初期化
    void Initialize(DirectXCommon* DirectXCollision, SrvManager* srvManager);

    void Update();

    void Draw();

    void CreateParticleGroup(const std::string name, const std::string textureFilePath, ParticleMeshType meshType);

    void Emit(const std::string name, const Transform& transform, uint32_t count, const EmitterParam& param);

    // set
    void SetDefaultCamera(Camera* camera) { this->Camera_ = camera; }

    ParticleManager(ParticleManager&) = delete;
    ParticleManager& operator=(ParticleManager&) = delete;

private:
    friend struct std::default_delete<ParticleManager>;
    ~ParticleManager() = default;

private:
    SrvManager* srvManager = nullptr;
    DirectXCommon* dxCommon = nullptr;
    WinApp* winApp_ = nullptr;

    Camera* Camera_ = nullptr;

    ModelData model;

    static std::unique_ptr<ParticleManager> instance;
    const uint32_t kMaxInstanceCount = 100;
    const float kDeltaTime = 1.0f / 60.0f;

    bool useBillboard = false;

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
