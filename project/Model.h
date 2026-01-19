#pragma once
#include "Math.h"
#include <d3d12.h>
#include <wrl.h>
class ModelCommon;

class Model {
public:
    // 初期化
    void Initialize(/*ModelCommon* modelCommon,*/ const std::string& directorypath, const std::string& filename);

    void Draw();

private:
    void VertexResourceInitialize();
    void MaterialResourceInitialize();

    ModelCommon* modelCommon_=nullptr;

    ModelData modelData;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    VertexData* vertexData = nullptr;

    // Model用のマテリアルリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
    // バッファリソース
    Material* materialData;
};
