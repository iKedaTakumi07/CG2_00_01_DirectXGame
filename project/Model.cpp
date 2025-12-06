#include "Model.h"
#include "ModelCommon.h"
#include "Object3d.h"
#include "TextureManager.h"

void Model::Initialize(ModelCommon* modelCommon)
{
    modelCommon_ = modelCommon;

    modelData = Object3d::LoadObjFile("resources", "plane.obj");

    // 頂点データ初期化
    VertexResourceInitialize();
    MaterialResourceInitialize();

    TextureManager::getInstance()->LoadTexture(modelData.material.textureFilePath);
    modelData.material.textureIndex = TextureManager::getInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);
}

void Model::Draw() {
    modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::getInstance()->GetSrvHandelGPU(modelData.material.textureIndex));
    modelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

void Model::VertexResourceInitialize()
{
    vertexResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
    // リソースの先端のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    // 使用するサイズ
    vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
    // 1ツ当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

void Model::MaterialResourceInitialize()
{
    // model用のマテリアルリソースを作る
    materialResource = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
    // mapして書き込み
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

    // 今回は白を書き込んでみる
    materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData->enableLighting = true;
    materialData->uvTransform = MakeIdentity4x4();
}