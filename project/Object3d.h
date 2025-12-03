#pragma once
#include "Math.h"
#include "Object3dCommon.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class Object3d {
public:
    struct MaterialData {
        std::string textureFilePath;
        uint32_t textureIndex = 0;
    };
    struct ModelData {
        std::vector<VertexData> vertices;
        MaterialData material;
    };
    struct DirectionalLight {
        Vector4 color;
        Vector3 direction;
        float intensity;
    };
    Matrix4x4 MakeIdentity4x4()
    {
        Matrix4x4 num;
        num = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
        return num;
    }

public:
    static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
    static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

    // 初期化
    void Initialize(Object3dCommon* object3dCommon);

    void Update();

private:
    void VertexResourceInitialize();
    void MaterialResourceInitialize();
    void TransMatrixResourceInitialize();
    void directionalLightInitialize();

    Object3dCommon* object3dCommon = nullptr;
    // objファイルのデータ
    ModelData modelData;

    // バッファリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
    VertexData* vertexData = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};

    // Model用のマテリアルリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
    // バッファリソース
    Material* materialData;

    // バッファリソース
    TransformationMatrix* transformationMatrixData;
    // model用のtransformmatrix用のリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;

    // 平行光源
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightMatrixResource;
    // データを書き込み
    DirectionalLight* directionalLightData = nullptr;

    Transform transform = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    Transform cameraTransform;
};
