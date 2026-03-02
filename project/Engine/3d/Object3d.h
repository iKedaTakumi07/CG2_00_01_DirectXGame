#pragma once
#include "../base/Math.h"
#include "../externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include <wrl.h>

class WinApp;
class Object3dCommon;
class Model;
class Camera;

class Object3d {
public:
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
    void Initialize();

    void Update();

    // 更新
    void Draw();

    // Setter
    void SetModel(Model* model) { this->model = model; }
    void SetScale(const Vector3& scale) { transform.scale = scale; }
    void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
    void SetTranslate(const Vector3& translate) { transform.translate = translate; }
    void SetModel(const std::string& filePath);
    void SetCamera(Camera* camera) { this->camera = camera; }

    // Getter
    const Vector3& GetScale() const { return transform.scale; }
    const Vector3& GetRotate() const { return transform.rotate; }
    const Vector3& GetTranslate() const { return transform.translate; }

private:
    void TransMatrixResourceInitialize();
    void directionalLightInitialize();

    Object3dCommon* object3dCommon = nullptr;
    WinApp* winApp_ = nullptr;
    Model* model = nullptr;
    Camera* camera = nullptr;

    // バッファリソース
    TransformationMatrix* transformationMatrixData;
    // model用のtransformMatrix用のリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;

    // 平行光源
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightMatrixResource;
    // データを書き込み
    DirectionalLight* directionalLightData = nullptr;

    Transform transform = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    Transform cameraTransform;
};
