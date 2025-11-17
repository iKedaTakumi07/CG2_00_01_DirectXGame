#include "Sprite.h"
#include "SpriteCommon.h"

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
    this->spriteCommon_ = spriteCommon;

    vertexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 4);
    // リソースの先端のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    // 使用するサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
    // 1ツ当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    // 1枚目
    vertexData[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
    vertexData[0].texcoord = { 0.0f, 1.0f };
    vertexData[0].normal = { 0.0f, 0.0f, 1.0f };
    vertexData[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexData[1].texcoord = { 0.0f, 0.0f };
    vertexData[1].normal = { 0.0f, 0.0f, 1.0f };
    vertexData[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
    vertexData[2].texcoord = { 1.0f, 1.0f };
    vertexData[2].normal = { 0.0f, 0.0f, 1.0f };
    vertexData[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
    vertexData[3].texcoord = { 1.0f, 0.0f };
    vertexData[3].normal = { 0.0f, 0.0f, 1.0f };

    // インデックスリソースにデータを書き込む
    indexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);
    // リソースの先頭のアドレスから使う
    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    // 使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
    // インデックスはuint32_Tとする
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    indexData[3] = 1;
    indexData[4] = 3;
    indexData[5] = 2;
}