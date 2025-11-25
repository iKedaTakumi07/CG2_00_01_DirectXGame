#pragma once
#include "Math.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class SpriteCommon;
class WinApp;
class TextureManager;

class Sprite {
public:
    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };

    struct Matrix4x4 {
        float m[4][4];
    };
    struct Material {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvTransform;
    };
    struct TransformationMatrix {
        Matrix4x4 WVP;
        Matrix4x4 world;
    };
    struct Transform {
        Vector3 scale;
        Vector3 rotate;
        Vector3 translate;
    };

    Matrix4x4 MakeIdentity4x4()
    {
        Matrix4x4 num;
        num = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
        return num;
    }
    Matrix4x4 MakeRotateXMatrix(float radian)
    {
        Matrix4x4 num;
        num = { 1, 0, 0, 0,
            0, std::cos(radian), std::sin(radian), 0,
            0, std::sin(-radian), std::cos(radian), 0,
            0, 0, 0, 1 };
        return num;
    }
    Matrix4x4 MakeRotateYMatrix(float radian)
    {
        Matrix4x4 num;
        num = { std::cos(radian), 0, std::sin(-radian), 0,
            0, 1, 0, 0,
            std::sin(radian), 0, std::cos(radian), 0,
            0, 0, 0, 1 };
        return num;
    }
    Matrix4x4 MakeRotateZMatrix(float radian)
    {
        Matrix4x4 num;
        num = { std::cos(radian), std::sin(radian), 0, 0,
            std::sin(-radian), std::cos(radian), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1 };
        return num;
    }
    Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2)
    {
        Matrix4x4 num;
        num.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
        num.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
        num.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
        num.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

        num.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
        num.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
        num.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
        num.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

        num.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
        num.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
        num.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
        num.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

        num.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
        num.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
        num.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
        num.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

        return num;
    }
    Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
    {
        Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);
        Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);
        Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);
        Matrix4x4 rotateXYZ = Multiply(rotateX, Multiply(rotateY, rotateZ));

        Matrix4x4 num;
        num.m[0][0] = scale.x * rotateXYZ.m[0][0];
        num.m[0][1] = scale.x * rotateXYZ.m[0][1];
        num.m[0][2] = scale.x * rotateXYZ.m[0][2];
        num.m[0][3] = 0.0f * 0.0f * 0.0f * 0.0f;
        num.m[1][0] = scale.y * rotateXYZ.m[1][0];
        num.m[1][1] = scale.y * rotateXYZ.m[1][1];
        num.m[1][2] = scale.y * rotateXYZ.m[1][2];
        num.m[1][3] = 0.0f * 0.0f * 0.0f * 0.0f;
        num.m[2][0] = scale.z * rotateXYZ.m[2][0];
        num.m[2][1] = scale.z * rotateXYZ.m[2][1];
        num.m[2][2] = scale.z * rotateXYZ.m[2][2];
        num.m[2][3] = 0.0f * 0.0f * 0.0f * 0.0f;
        num.m[3][0] = translate.x;
        num.m[3][1] = translate.y;
        num.m[3][2] = translate.z;
        num.m[3][3] = 1.0f;
        return num;
    }
    Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
    {
        Matrix4x4 num;
        num = { 2 / (right - left), 0, 0, 0, 0, 2 / (top - bottom), 0, 0, 0, 0, 1 / (farClip - nearClip), 0, (left + right) / (left - right),
            (top + bottom) / (bottom - top),
            nearClip / (nearClip - farClip), 1 };
        return num;
    }

    // 初期化
    void Initialize(SpriteCommon* spriteCommon, WinApp* winApp, std::string texturefilePath);

    // 更新
    void Update();

    // 更新
    void Draw();

    // getter
    const Vector2& GetPosition() const { return position; }
    float GetRotation() const { return rotation; }
    const Vector4& GetColor() { return materialData->color; }
    const Vector2& GetSize() const { return size; }
    const uint32_t GettextureIndex() { return textureIndex; }
    // setter
    void SetPosition(const Vector2& position) { this->position = position; }
    void SetRotation(float rotation) { this->rotation = rotation; }
    void SetColor(const Vector4& color) { materialData->color = color; }
    void SetSize(const Vector2& size) { this->size = size; }

private:
    void VertexResourceInitialize();
    void MaterialResourceInitialize();
    void TransMatrixResourceInitialize();

    SpriteCommon* spriteCommon_ = nullptr;
    WinApp* winApp_ = nullptr;

    // Sprite用の頂点リソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

    VertexData* vertexData = nullptr;
    uint32_t* indexData = nullptr;

    // 頂点バッファビューを作る
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    D3D12_INDEX_BUFFER_VIEW indexBufferView {};

    // Sprite用のマテリアルリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

    // バッファリソース
    Material* materialData;

    // バッファリソース
    TransformationMatrix* transformationMatrixData;

    // Sprite用のtransformmatrix用のリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;

    // 動かす用のtransform
    Transform transform { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

    Vector2 position = { 0.0f, 0.0f };
    float rotation = 0.0f;
    Vector2 size { 640.0f, 360.0f };

    uint32_t textureIndex = 0;
};
