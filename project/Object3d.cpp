#include "Object3d.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>

Object3d::MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
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

Object3d::ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
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
                position.x *= -1.0f;
                texcoord.y = 1.0f - texcoord.y;
                normal.x *= -1.0f;

                /* VertexData vertex = { position, texcoord, normal };
                 modelData.vertices.push_back(vertex);*/

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

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
    this->object3dCommon = object3dCommon;

    modelData = LoadObjFile("resources", "plane.obj");

    VertexResourceInitialize();
    MaterialResourceInitialize();
    TransMatrixResourceInitialize();
    directionalLightInitialize();

    TextureManager::getInstance()->LoadTexture(modelData.material.textureFilePath);
    modelData.material.textureIndex = TextureManager::getInstance()->GetTextureIndexByFilePath(modelData.material.textureFilePath);

    transform = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    cameraTransform = { { 1.0f, 1.0f, 1.0f }, { 0.3f, 0.0f, 0.0f }, { 0.0f, 4.0f, -10.0f } };
}

void Object3d::Update()
{
    Matrix4x4 worldMatrixModel = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
    Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
    Matrix4x4 projectionMatrixModel = MakePrespectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
    Matrix4x4 worldViewProjectionMatrixModel = Multiply(worldMatrixModel, Multiply(viewMatrix, projectionMatrixModel));
    transformationMatrixData->WVP = worldViewProjectionMatrixModel;
    transformationMatrixData->world = worldMatrixModel;

    directionalLightData->direction = Normalize(directionalLightData->direction);
}

void Object3d::VertexResourceInitialize()
{
    vertexResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
    // リソースの先端のアドレスから使う
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    // 使用するサイズ
    vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
    // 1ツ当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
}
void Object3d::MaterialResourceInitialize()
{
    // model用のマテリアルリソースを作る
    materialResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));
    // mapして書き込み
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

    // 今回は白を書き込んでみる
    materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    materialData->enableLighting = false;
    materialData->uvTransform = MakeIdentity4x4();
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
