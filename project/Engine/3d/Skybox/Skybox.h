#pragma once
#include "../../base/Math.h"
class SkyBoxCommon;
class SpriteCommon;

class Skybox {
public:
    // 初期化
    void Initialize(std::string texturefilePath);

    // 更新
    void Draw();

private:
    void VertexResourceInitialize();
    void MaterialResourceInitialize();
    void TransMatrixResourceInitialize();

    SkyBoxCommon* SkyBoxCommon_ = nullptr;

    VertexData* vertexData = nullptr;

    std::string texturefilePath_;
};
