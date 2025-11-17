#pragma once
#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include <wrl.h>

class SpriteCommon;

class Sprite {
public:
    struct Vector2 {
        float x;
        float y;
    };
    struct Vector3 {
        float x;
        float y;
        float z;
    };
    struct Vector4 {
        float x;
        float y;
        float z;
        float w;
    };
    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };

    // 初期化
    void Initialize(SpriteCommon* spriteCommon);

private:
    SpriteCommon* spriteCommon_ = nullptr;

    // Sprite用の頂点リソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

    VertexData* vertexData = nullptr;
    uint32_t* indexData = nullptr;

    // 頂点バッファビューを作る
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    D3D12_INDEX_BUFFER_VIEW indexBufferView {};
};
