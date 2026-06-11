#include "Object3d.h"
#include "../base/TextureManager.h"
#include "Camera.h"
#include "Model.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <cassert>
#include <fstream>
#include <numbers>
#ifdef USE_IMGUI
#include "../externals/imgui/imgui.h"
#endif // USE_IMGUI
#include <cmath>

// MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
//{
//     MaterialData materialData; // 構築するMaterialData
//     std::string line; // ファイルから読み込んだ1行を格納するもの
//     std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
//     assert(file.is_open()); // 開けられないなら止める
//
//     while (std::getline(file, line)) {
//         std::string identifier;
//         std::istringstream s(line);
//         s >> identifier;
//
//         // identifierに応じた処理
//         if (identifier == "map_Kd") {
//             std::string textureFilename;
//             s >> textureFilename;
//             // 連結してファイルパスにする
//             materialData.textureFilePath = directoryPath + "/" + textureFilename;
//         }
//     }
//     return materialData;
// }

ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
    ModelData modelData; // 構築するmodelData

    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + filename;
    const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);
    assert(scene->HasMeshes());

    uint32_t vertexOffset = 0;

    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        assert(mesh->HasNormals());
        assert(mesh->HasTextureCoords(0));

        // メッシュ頂点データ
        for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
            aiVector3D& position = mesh->mVertices[vertexIndex];
            aiVector3D& normal = mesh->mNormals[vertexIndex];
            aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

            VertexData vertex;
            vertex.position = { position.x, position.y, position.z, 1.0f };
            vertex.normal = { normal.x, normal.y, normal.z };
            vertex.texcoord = { texcoord.x, texcoord.y };

            // aiProcess_FlipWindingOrderはz*=-1,右手->左手に変換するので手動で対処
            vertex.position.x *= -1.0f;
            vertex.normal.x *= -1.0f;

            modelData.vertices.push_back(vertex);
        }

        // Face解析
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3);

            // veertex解析
            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t globalVertexIndex = face.mIndices[element] + vertexOffset;
                modelData.indices.push_back(globalVertexIndex);
            }
        }
        vertexOffset += mesh->mNumVertices;
    }

    // マテリアる
    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString texttureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texttureFilePath);
            modelData.material.textureFilePath = directoryPath + "/" + texttureFilePath.C_Str();
        }
    }

    modelData.rootNode = ReadNode(scene->mRootNode);

    return modelData;
}

Animation Object3d::LoadAinmationFile(const std::string& directoryPath, const std::string& filename)
{
    Animation animation; // 構築するアニメーション

    Assimp::Importer importer;
    std::string filePath = directoryPath + "/" + filename;
    const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
    assert(scene->mNumAnimations != 0);
    aiAnimation* animationAssimp = scene->mAnimations[0]; // 最初のアニメーションだけ採用。//[後日]複数対応予定。
    animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変更

    // nodeAnimationを解析
    for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
        aiNodeAnim* nodeAnimatonAssimp = animationAssimp->mChannels[channelIndex];
        NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimatonAssimp->mNodeName.C_Str()];

        // 座標
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimatonAssimp->mNumPositionKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimatonAssimp->mPositionKeys[keyIndex];
            keyframeVector3 keyframe;
            keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); // 秒に変換
            keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手

            nodeAnimation.translate.keyframes.push_back(keyframe);
        }

        // 回転
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimatonAssimp->mNumRotationKeys; ++keyIndex) {
            aiQuatKey& keyAssimp = nodeAnimatonAssimp->mRotationKeys[keyIndex];
            keyframeQuaternion keyframe;
            keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);

            keyframe.value = {
                keyAssimp.mValue.x,
                -keyAssimp.mValue.y,
                -keyAssimp.mValue.z,
                keyAssimp.mValue.w
            };

            nodeAnimation.rotate.keyframes.push_back(keyframe);
        }

        // 縮尺
        for (uint32_t keyIndex = 0; keyIndex < nodeAnimatonAssimp->mNumScalingKeys; ++keyIndex) {
            aiVectorKey& keyAssimp = nodeAnimatonAssimp->mScalingKeys[keyIndex];
            keyframeVector3 keyframe;
            keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);

            keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };

            nodeAnimation.scale.keyframes.push_back(keyframe);
        }
    }

    return animation;
}

Node Object3d::ReadNode(aiNode* node)
{
    Node result;
    aiMatrix4x4 aiLocalMatrix = node->mTransformation;
    aiLocalMatrix.Transpose();
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            result.localMatrix.m[x][y] = aiLocalMatrix[x][y];
        }
    }

    result.name = node->mName.C_Str(); // node名を格納
    result.childrem.resize(node->mNumChildren); // 子の数だけ確保

    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
        // 再帰関数
        result.childrem[childIndex] = ReadNode(node->mChildren[childIndex]);
    }

    return result;
}

void Object3d::PlayAnimation(const std::string& directoryPath, const std::string& filename)
{
    animation_ = LoadAinmationFile(directoryPath, filename);
    animationTime_ = 0.0f;
    isAnimating_ = true;
}

void Object3d::Initialize()
{
    this->object3dCommon = Object3dCommon::GetInstance();

    this->winApp_ = WinApp::GetInstance();

    this->camera = object3dCommon->GetDefaultCamera();

    transform = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    cameraTransform = { { 1.0f, 1.0f, 1.0f }, { 0.3f, 0.0f, 0.0f }, { 0.0f, 4.0f, -10.0f } };

    TransMatrixResourceInitialize();
    directionalLightInitialize();
    cameraDataResourceInitialize();
    pointLightInitialize();
    spotLightInitialize();
}

void Object3d::Update()
{
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

    if (model) {
        const ModelData& modelData = model->GetModelData();

        Matrix4x4 localMatrix = modelData.rootNode.localMatrix;

        if (isAnimating_) {
            animationTime_ += 1.0f / 60.0f;
            animationTime_ = std::fmod(animationTime_, animation_.duration);
            NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData.rootNode.name];
            Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
            Vector4 rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
            Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
            Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);
        }

        worldMatrix = Multiply(localMatrix, worldMatrix);
    }

    Matrix4x4 worldViewProjectionMatrix;
    const Matrix4x4& ViewProjectionMatrix = camera->GetViewProjectionMatrix();
    worldViewProjectionMatrix = Multiply(worldMatrix, ViewProjectionMatrix);

    transformationMatrixData->WVP = worldViewProjectionMatrix;
    transformationMatrixData->world = worldMatrix;
    transformationMatrixData->worldInverseTranspose = Transpose(Inverse(worldMatrix));

    directionalLightData->direction = Normalize(directionalLightData->direction);
}

void Object3d::DrawImGui(const std::string& label)
{
#ifdef USE_IMGUI
    ImGui::Begin("Objects Control");

    ImGui::PushID(label.c_str());

    if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
        ImGui::SliderAngle("RotateX", &transform.rotate.x);
        ImGui::SliderAngle("RotateY", &transform.rotate.y);
        ImGui::SliderAngle("RotateZ", &transform.rotate.z);

        // --- Directional Light ---
        if (ImGui::CollapsingHeader("Directional Light")) {
            ImGui::SliderFloat3("direction", &directionalLightData->direction.x, -1.0f, 1.0f);
            ImGui::ColorEdit4("Color", &(directionalLightData->color).x);
            ImGui::DragFloat("intensity", &directionalLightData->intensity, 0.01f);
        }

        // --- Point Light ---
        if (ImGui::CollapsingHeader("Point Light")) {
            ImGui::ColorEdit4("color", &(PointLigthData->color).x);
            ImGui::DragFloat3("Position", &PointLigthData->position.x, 0.01f);
            ImGui::DragFloat("radius", &PointLigthData->radius, 0.01f);
            ImGui::DragFloat("intensity", &PointLigthData->intensity, 0.01f);
            ImGui::DragFloat("decay", &PointLigthData->decay, 0.01f);
        }

        // --- Spot Light ---
        if (ImGui::CollapsingHeader("Spot Light")) {
            ImGui::ColorEdit4("color", &(SpotLigthData->color).x);
            ImGui::DragFloat3("position", &SpotLigthData->position.x, 0.01f);
            ImGui::DragFloat("intensity", &SpotLigthData->intensity, 0.01f);
            ImGui::DragFloat3("direction", &SpotLigthData->direction.x, 0.01f);
            ImGui::DragFloat("distance", &SpotLigthData->distance, 0.01f);
            ImGui::DragFloat("decay", &SpotLigthData->decay, 0.01f);
            ImGui::DragFloat("cosAngle", &SpotLigthData->cosAngle, 0.01f);
            ImGui::DragFloat("cosFalloffStart", &SpotLigthData->cosFalloffStart, 0.01f);
        }

        if (ImGui::CollapsingHeader("materialData")) {
            Material* materialData = model->GetmaterialData();
            bool isLighting = (materialData->enableLighting != 0);
            if (ImGui::Checkbox("Enable Lighting (Unlit Texture)", &isLighting)) {
                materialData->enableLighting = isLighting ? 1 : 0;
            }
            ImGui::DragFloat("evnironmentCoefficient", &materialData->evnironmentCoefficient, 0.01f);
            model->SetMaterialDataEvnironmentCoefficient(materialData->evnironmentCoefficient);
        }
    }

    ImGui::PopID();

    ImGui::End();
#endif // USE_IMGUI
}

void Object3d::Draw()
{
    object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
    object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightMatrixResource->GetGPUVirtualAddress());
    object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, CameraDataResourceModel->GetGPUVirtualAddress());
    object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLigth->GetGPUVirtualAddress());
    object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLigth->GetGPUVirtualAddress());
    if (model) {
        model->Draw();
    }
}

void Object3d::SetModel(const std::string& filePath)
{
    // モデル検索してセット
    model = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::TransMatrixResourceInitialize()
{
    transformationMatrixResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

    // 書き込むためのアドレス取得
    transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
    // 単位行列を書き込む
    transformationMatrixData->WVP = MakeIdentity4x4();
    transformationMatrixData->world = MakeIdentity4x4();
    transformationMatrixData->worldInverseTranspose = MakeIdentity4x4();
}

void Object3d::directionalLightInitialize()
{
    directionalLightMatrixResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
    // アドレスを取得
    directionalLightMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    // 書き込み
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = { 0.0f, 0.0f, -1.0f };
    directionalLightData->intensity = 1.0f;
}

void Object3d::cameraDataResourceInitialize()
{
    // sphere用のマテリアルリソースを作る
    CameraDataResourceModel = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));

    // mapして書き込み
    CameraDataResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&CameraForGPUData));
    // 今回は白を書き込んでみる
    CameraForGPUData->worldPosition = cameraTransform.translate;
}

void Object3d::pointLightInitialize()
{
    pointLigth = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(PointLigth));

    // mapして書き込み
    pointLigth->Map(0, nullptr, reinterpret_cast<void**>(&PointLigthData));
    // 今回は白を書き込んでみる
    PointLigthData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    PointLigthData->position = Vector3(0.0f, 2.0f, 0.0f);
    PointLigthData->intensity = 0.0f;
    PointLigthData->decay = 1.0f;
    PointLigthData->radius = 0.0f;
}

void Object3d::spotLightInitialize()
{
    spotLigth = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(SpotLigth));

    // mapして書き込み
    spotLigth->Map(0, nullptr, reinterpret_cast<void**>(&SpotLigthData));
    // 今回は白を書き込んでみる
    SpotLigthData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    SpotLigthData->position = Vector3(0.0f, 2.0f, 0.0f);
    SpotLigthData->distance = 1.0f;
    SpotLigthData->direction = Normalize({ -1.0f, -1.0f, 0.0f });
    SpotLigthData->intensity = 0.0f;
    SpotLigthData->decay = 2.0f;
    SpotLigthData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
    SpotLigthData->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);
}
