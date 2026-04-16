#include "Skybox.h"
#include "../../2d/SpriteCommon.h"
#include "SkyBoxCommon.h"

void Skybox::Initialize(std::string texturefilePath)
{
    SkyBoxCommon_ = SkyBoxCommon::GetInstance();

    texturefilePath_ = texturefilePath;

    // 頂点データ
    VertexResourceInitialize();
    MaterialResourceInitialize();
    TransMatrixResourceInitialize();
}

void Skybox::VertexResourceInitialize()
{
}

void Skybox::MaterialResourceInitialize()
{
}

void Skybox::TransMatrixResourceInitialize()
{
}
