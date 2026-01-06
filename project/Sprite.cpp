#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif // USE_IMGUI

void Sprite::Initialize(SpriteCommon* spriteCommon, WinApp* winApp, std::string texturefilePath)
{
    this->spriteCommon_ = spriteCommon;
    this->winApp_ = winApp;

    VertexResourceInitialize();
    MaterialResourceInitialize();
    TransMatrixResourceInitialize();

    // textureIndex = TextureManager::getInstance()->GetTextureIndexByFilePath(texturefilePath);
    texturefilePath_ = texturefilePath;
    AdjustTextureSize(texturefilePath_);
}

void Sprite::VertexResourceInitialize()
{

    vertexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * 4);
    // リソースの先端のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    // 使用するサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
    // 1ツ当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    // インデックスリソースにデータを書き込む
    indexResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);
    // リソースの先頭のアドレスから使う
    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    // 使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
    // インデックスはuint32_Tとする
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Sprite::MaterialResourceInitialize()
{
    // Sprite用のマテリアルリソースを作る
    materialResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
    // mapして書き込み
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

    // 今回は白を書き込んでみる
    materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    materialData->enableLighting = false;
    materialData->uvTransform = MakeIdentity4x4();
}

void Sprite::TransMatrixResourceInitialize()
{

    transformationMatrixResource = spriteCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

    // 書き込むためのアドレス取得
    transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
    // 単位行列を書き込む
    transformationMatrixData->WVP = MakeIdentity4x4();
    transformationMatrixData->world = MakeIdentity4x4();
}

void Sprite::AdjustTextureSize(std::string texturefilePath)
{
    // テクスチャメタデータを取得
    const DirectX::TexMetadata& metadata = TextureManager::getInstance()->GetMetadata(texturefilePath);

    textureSize.x = static_cast<float>(metadata.width);
    textureSize.y = static_cast<float>(metadata.height);
    // 画像サイズをテクスチャサイズに合わせる
    size = textureSize;
}

void Sprite::Update()
{
#ifdef USE_IMGUI
    ImGui::SliderFloat2("pos", &position.x, 0.0f, 1280.0f, "%06.1f");
#endif // USE_IMGUI

    float left = 0.0f - anchorPoint.x;
    float rigth = 1.0f - anchorPoint.x;
    float top = 0.0f - anchorPoint.y;
    float bottom = 1.0f - anchorPoint.y;

    // 左右反転,上下反転
    if (isFlipX_) {
        left = -left;
        rigth = -rigth;
    }
    if (isFlipY_) {
        top = -top;
        bottom = -bottom;
    }

    const DirectX::TexMetadata& metadata = TextureManager::getInstance()->GetMetadata(texturefilePath_);
    float tex_left = textureLeftTop.x / metadata.width;
    float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
    float tex_top = textureLeftTop.y / metadata.height;
    float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;

    // 頂点リソースにデータを書き込む
    vertexData[0].position = { left, bottom, 0.0f, 1.0f };
    vertexData[0].texcoord = { tex_left, tex_bottom };
    vertexData[0].normal = { 0.0f, 0.0f, -1.0f };
    vertexData[1].position = { left, top, 0.0f, 1.0f };
    vertexData[1].texcoord = { tex_left, tex_top };
    vertexData[1].normal = { 0.0f, 0.0f, -1.0f };
    vertexData[2].position = { rigth, bottom, 0.0f, 1.0f };
    vertexData[2].texcoord = { tex_right, tex_bottom };
    vertexData[2].normal = { 0.0f, 0.0f, -1.0f };
    vertexData[3].position = { rigth, top, 0.0f, 1.0f };
    vertexData[3].texcoord = { tex_right, tex_top };
    vertexData[3].normal = { 0.0f, 0.0f, -1.0f };
    // インデックスリソースにデータを書きこむ
    indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    indexData[3] = 1;
    indexData[4] = 3;
    indexData[5] = 2;

    // sprite用
    transform.translate = { position.x, position.y, 0.0f };
    transform.rotate = { 0.0f, 0.0f, rotation };
    transform.scale = { size.x, size.y, 1.0f };

    Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
    Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
    Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(winApp_->KClientWidth), float(winApp_->KClientHeight), 0.0f, 100.0f);
    Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrix, Multiply(viewMatrixSprite, projectionMatrixSprite));
    transformationMatrixData->WVP = worldViewProjectionMatrixSprite;
    transformationMatrixData->world = worldMatrix;
}

void Sprite::Draw()
{
    // Spriteの描画
    // VertexBufferView
    spriteCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    // IndexBufferView
    spriteCommon_->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);

    // マテリアルCBufferの場所を設定
    spriteCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    // wvp用のCBufferの場所を設定
    spriteCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

    spriteCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::getInstance()->GetSrvHandelGPU(texturefilePath_));

    // 描画
    spriteCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
