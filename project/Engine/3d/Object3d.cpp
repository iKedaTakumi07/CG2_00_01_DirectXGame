#include "Object3d.h"
#include "../base/TextureManager.h"
#include "../scene/SceneManager.h"
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

    size_t lastSlash = filePath.find_last_of("/\\");
    std::string modelDirectory = (lastSlash != std::string::npos) ? filePath.substr(0, lastSlash) : directoryPath;

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

            std::string texName = texttureFilePath.C_Str();
            modelData.material.textureFilePath = modelDirectory + "/" + texName;
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
    aiVector3D scale, translate;
    aiQuaternion rotate;
    node->mTransformation.Decompose(scale, rotate, translate);
    result.transfrom.scale = { scale.x, scale.y, scale.z };
    result.transfrom.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; // x軸反転、回転方向が逆なので軸を反転させる
    result.transfrom.translate = { -translate.x, translate.y, translate.z }; // x軸反転
    result.localMatrix = MakeAffineMatrix(result.transfrom.scale, result.transfrom.rotate, result.transfrom.translate);

    result.name = node->mName.C_Str(); // node名を格納
    result.childrem.resize(node->mNumChildren); // 子の数だけ確保

    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
        // 再帰関数
        result.childrem[childIndex] = ReadNode(node->mChildren[childIndex]);
    }

    return result;
}

void Object3d::LoadAnimation(const std::string& directoryPath, const std::string& filename, const std::string& animName)
{
    // すでに同じ名前で登録されている場合はスキップ
    if (animation_.find(animName) != animation_.end()) {
        return;
    }
    // アニメーションファイルをロードしてマップに登録
    animation_[animName] = LoadAinmationFile(directoryPath, filename);
}

void Object3d::PlayAnimation(const std::string& animName, bool loop)
{
    auto it = animation_.find(animName);
    assert(it != animation_.end() && "指定されたアニメーションはロードされていません。");

    currentAnimation_ = &it->second;
    currentAnimationName_ = animName;
    animationTime_ = 0.0f;
    isAnimating_ = true;
    isLoop_ = loop;
}

void Object3d::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
{
    for (Joint& joint : skeleton.joints) {
        // 対象のjointのanimationがあれば、値の適応を行う。下記のif分はC++17から可能な奴
        if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
            const NodeAnimation& rootNodeAnimaiton = (*it).second;
            joint.transform.translate = CalculateValue(rootNodeAnimaiton.translate.keyframes, animationTime);
            joint.transform.rotate = CalculateValue(rootNodeAnimaiton.rotate.keyframes, animationTime);
            joint.transform.scale = CalculateValue(rootNodeAnimaiton.scale.keyframes, animationTime);
        }
    }
}

void Object3d::StopAnimation()
{
    isAnimating_ = false;
    currentAnimation_ = nullptr;
    currentAnimationName_ = "";
    animationTime_ = 0.0f;
}

Skeleton Object3d::CreateSkeleton(const Node& rootNode)
{
    Skeleton skeleton;
    skeleton.root = CreateJoint(rootNode, { }, skeleton.joints);

    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    return skeleton;
}

int32_t Object3d::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
    Joint joint;
    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = MakeIdentity4x4();
    joint.transform = node.transfrom;
    joint.index = int32_t(joints.size()); // 現在登録されている数をindex
    joint.parent = parent;
    joints.push_back(joint);
    for (const Node& child : node.childrem) {
        // 子jointを作成,index登録
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].childern.push_back(childIndex);
    }
    // 自身のindexを返す
    return joint.index;
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

#ifdef USE_IMGUI
    InitializeSkeletonBuffer();
#endif
}

void Object3d::Update()
{
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

    if (model) {
        if (isAnimating_ && currentAnimation_) {
            float deltaTime = SceneManager::GetInstance()->GetDeltaTime();
            animationTime_ += deltaTime;

            if (isLoop_) {
                animationTime_ = std::fmod(animationTime_, currentAnimation_->duration);
            } else {
                // 単発再生
                if (animationTime_ >= currentAnimation_->duration) {
                    // 最後に達したら再生停止
                    animationTime_ = currentAnimation_->duration;
                    isAnimating_ = false;
                }
            }
            // スケルトンに現在のアニメーションを適応
            ApplyAnimation(skeleton_, *currentAnimation_, animationTime_);
        }

        // スケルトンの行列変換
        Update(skeleton_);

        Matrix4x4 localMatrix = MakeIdentity4x4();
        if (!skeleton_.joints.empty()) {
            localMatrix = skeleton_.joints[skeleton_.root].skeletonSpaceMatrix;
        } else {
            // スケルトンが無い場合は元のノードの行列を使う（フォールバック）
            localMatrix = model->GetModelData().rootNode.localMatrix;
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

void Object3d::Update(Skeleton& skeleton)
{
    // すべてのjointを更新。親がわっ回ので通常ループで処理可
    for (Joint& joint : skeleton.joints) {
        joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
        if (joint.parent) { // 親がいれば親の行列をかける
            joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
        } else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }
}

void Object3d::DrawImGui(const std::string& label)
{
#ifdef USE_IMGUI
    ImGui::Begin("Objects Control");
    ImGui::PushID(label.c_str());

    if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::Indent();

        ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
        ImGui::SliderAngle("RotateX", &transform.rotate.x);
        ImGui::SliderAngle("RotateY", &transform.rotate.y);
        ImGui::SliderAngle("RotateZ", &transform.rotate.z);
        ImGui::Spacing();

        // --- Directional Light ---
        if (ImGui::CollapsingHeader("Directional Light")) {
            ImGui::Indent();
            ImGui::SliderFloat3("direction", &directionalLightData->direction.x, -1.0f, 1.0f);
            ImGui::ColorEdit4("Color", &(directionalLightData->color).x);
            ImGui::DragFloat("intensity", &directionalLightData->intensity, 0.01f);
            ImGui::Unindent();
        }

        // --- Point Light ---
        if (ImGui::CollapsingHeader("Point Light")) {
            ImGui::Indent();
            ImGui::ColorEdit4("color", &(PointLigthData->color).x);
            ImGui::DragFloat3("Position", &PointLigthData->position.x, 0.01f);
            ImGui::DragFloat("radius", &PointLigthData->radius, 0.01f);
            ImGui::DragFloat("intensity", &PointLigthData->intensity, 0.01f);
            ImGui::DragFloat("decay", &PointLigthData->decay, 0.01f);
            ImGui::Unindent();
        }

        // --- Spot Light ---
        if (ImGui::CollapsingHeader("Spot Light")) {
            ImGui::Indent();
            ImGui::ColorEdit4("color", &(SpotLigthData->color).x);
            ImGui::DragFloat3("position", &SpotLigthData->position.x, 0.01f);
            ImGui::DragFloat("intensity", &SpotLigthData->intensity, 0.01f);
            ImGui::DragFloat3("direction", &SpotLigthData->direction.x, 0.01f);
            ImGui::DragFloat("distance", &SpotLigthData->distance, 0.01f);
            ImGui::DragFloat("decay", &SpotLigthData->decay, 0.01f);
            ImGui::DragFloat("cosAngle", &SpotLigthData->cosAngle, 0.01f);
            ImGui::DragFloat("cosFalloffStart", &SpotLigthData->cosFalloffStart, 0.01f);
            ImGui::Unindent();
        }

        if (ImGui::CollapsingHeader("materialData")) {
            ImGui::Indent();
            Material* materialData = model->GetmaterialData();
            bool isLighting = (materialData->enableLighting != 0);
            if (ImGui::Checkbox("Enable Lighting (Unlit Texture)", &isLighting)) {
                materialData->enableLighting = isLighting ? 1 : 0;
            }
            ImGui::DragFloat("evnironmentCoefficient", &materialData->evnironmentCoefficient, 0.01f);
            model->SetMaterialDataEvnironmentCoefficient(materialData->evnironmentCoefficient);
            ImGui::Unindent();
        }

        ImGui::Unindent();
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

void Object3d::SetModel(Model* model)
{
    this->model = model;

    // スケルトン構築
    if (this->model) {
        skeleton_ = CreateSkeleton(this->model->GetModelData().rootNode);
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

#ifdef USE_IMGUI
void Object3d::InitializeSkeletonBuffer()
{
    // 100ジョイント分(100本)の線を想定
    uint32_t maxLines = 100;
    uint32_t bufferSize = sizeof(LineVertex) * maxLines * 2;

    // ヒーププロパティの設定 (アップロードヒープ)
    D3D12_HEAP_PROPERTIES heapProps { };
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    // リソース記述
    D3D12_RESOURCE_DESC resDesc { };
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = bufferSize;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.Format = DXGI_FORMAT_UNKNOWN;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // リソースの生成
    auto device = object3dCommon->GetDxCommon()->GetDevice();
    device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&skeletonVertexBuffer_));

    // ビューの作成
    skeletonVertexBufferView_.BufferLocation = skeletonVertexBuffer_->GetGPUVirtualAddress();
    skeletonVertexBufferView_.SizeInBytes = (UINT)bufferSize;
    skeletonVertexBufferView_.StrideInBytes = sizeof(LineVertex);
}

void Object3d::UpdateSkeletonLines()
{
    if (skeleton_.joints.empty())
        return;

    skeletonVertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&lineVertices));

    skeletonLineCount_ = 0;
    for (const Joint& joint : skeleton_.joints) {
        if (joint.parent) {
            // 親の位置 (ローカルの累積行列の4列目)
            const Matrix4x4& parentMat = skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
            Vector3 parentPos = { parentMat.m[3][0], parentMat.m[3][1], parentMat.m[3][2] };

            // 自分の位置
            const Matrix4x4& childMat = joint.skeletonSpaceMatrix;
            Vector3 childPos = { childMat.m[3][0], childMat.m[3][1], childMat.m[3][2] };

            // 頂点にセット（ワールド行列の適用はシェーダー側で行うため、ここではそのまま）
            lineVertices[skeletonLineCount_ * 2 + 0].position = { parentPos.x, parentPos.y, parentPos.z, 1.0f };
            lineVertices[skeletonLineCount_ * 2 + 1].position = { childPos.x, childPos.y, childPos.z, 1.0f };
            skeletonLineCount_++;
        }
    }
    skeletonVertexBuffer_->Unmap(0, nullptr);
}

void Object3d::DrawSkeleton()
{
    if (skeletonLineCount_ == 0)
        return;

    auto cmdList = object3dCommon->GetDxCommon()->GetCommandList();

    cmdList->IASetVertexBuffers(0, 1, &skeletonVertexBufferView_);

    // b0レジスタに WVP行列（transformationMatrixResource）を渡す
    cmdList->SetGraphicsRootConstantBufferView(0, transformationMatrixResource->GetGPUVirtualAddress());

    // 1本の線につき2頂点なので、総頂点数は LineCount * 2
    cmdList->DrawInstanced(skeletonLineCount_ * 2, 1, 0, 0);
}

#endif // USE_IMGUI