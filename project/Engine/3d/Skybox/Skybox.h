#pragma once
#include "../../base/Math.h"
class SkyBoxCommon;
class SpriteCommon;

class Skybox {
public:
    // 初期化
    void Initialize(std::string texturefilePath);

    // 更新
    void Draw();

private:
    void VertexResourceInitialize();
    void MaterialResourceInitialize();
    void TransMatrixResourceInitialize();

    SkyBoxCommon* SkyBoxCommon_ = nullptr;
    Camera* camera = nullptr;

    VertexData* vertexData = nullptr;

    // Skybox用のマテリアルリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
    // バッファリソース
    Material* materialData;

    // 頂点バッファビューを作る
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    D3D12_INDEX_BUFFER_VIEW indexBufferView {};
    // Skybox用の頂点リソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

    // バッファリソース
    TransformationMatrix* transformationMatrixData;

    // Skybox用のtransformMatrix用のリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;

    uint32_t textureIndex_ = 0;
    std::string texturefilePath_;
};
