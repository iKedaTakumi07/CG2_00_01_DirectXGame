#include "Object3d.h"
#include "Model.h"
#include "ModelManager.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>

MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
    MaterialData materialData; // 構築するMaterialData
    std::string line; // ファイルから読み込んだ1行を格納するもの
    std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
    assert(file.is_open()); // 開けられないなら止める

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // identfierに応じた処理
        if (identifier == "map_Kd") {
            std::string textureFilename;
            s >> textureFilename;
            // 連結してファイルパスにする
            materialData.textureFilePath = directoryPath + "/" + textureFilename;
        }
    }
    return materialData;
}

ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
    ModelData modelData; // 構築するmodeldata
    std::vector<Vector4> positions; // 位置
    std::vector<Vector3> normals; // 法線
    std::vector<Vector2> texcoords; // テクスチャ座標
    std::string line; // ファイルから読んだ1行を格納するもの

    // ファイルを開く
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open()); // 開けられないなら止める

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier; // 先頭の識別子を読む

        // identifierに応じた処理
        if (identifier == "v") {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.w = 1.0f;
            positions.push_back(position);
        } else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);
        } else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (identifier == "f") {
            // 面は三角形限定,その他未対応

            VertexData triangle[3];

            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                // 頂点の要素へのindexは[位置/uv/法線]で格納されているので.分割してindexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndeices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/'); // 区切りインデクスを読んでいく
                    elementIndeices[element] = std::stoi(index);
                }
                // 要素へのindexから,実際の要素の値を取得して,頂点を構築する
                Vector4 position = positions[elementIndeices[0] - 1];
                Vector2 texcoord = texcoords[elementIndeices[1] - 1];
                Vector3 normal = normals[elementIndeices[2] - 1];

                // 位置の反転&法線の反転&左下原点

                texcoord.y = 1.0f - texcoord.y;

                // VertexData vertex = { position, texcoord, normal };
                // modelData.vertices.push_back(vertex);

                triangle[faceVertex] = { position, texcoord, normal };
            }
            // 頂点を逆順で登録することで、周り順を逆にする
            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        } else if (identifier == "mtllib") {
            // materialTemplateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            // 基本的のobjファイルと同一階級にmtlは存在させるので,ディレクトリ名とファイル名を返す
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }

    return modelData;
}

void Object3d::Initialize(Object3dCommon* object3dCommon, WinApp* winApp)
{
    this->object3dCommon = object3dCommon;
    this->winApp_ = winApp;

    transform = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    cameraTransform = { { 1.0f, 1.0f, 1.0f }, { 0.3f, 0.0f, 0.0f }, { 0.0f, 4.0f, -10.0f } };

    TransMatrixResourceInitialize();
    directionalLightInitialize();
}

void Object3d::Update()
{
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
    Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
    Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(winApp_->KClientWidth) / float(winApp_->KClientHeight), 0.1f, 100.0f);
    Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
    transformationMatrixData->WVP = worldViewProjectionMatrix;
    transformationMatrixData->world = worldMatrix;

    directionalLightData->direction = Normalize(directionalLightData->direction);
}

void Object3d::Draw()
{
    object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
    object3dCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightMatrixResource->GetGPUVirtualAddress());

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
}

void Object3d::directionalLightInitialize()
{
    directionalLightMatrixResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
    // アドレスを取得
    directionalLightMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    // 書き込み
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
    directionalLightData->intensity = 1.0f;
}
