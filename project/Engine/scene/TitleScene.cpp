#include "TitleScene.h"
#include "GamePlayScene.h"
#include "SceneManager.h"

#include "../3d/Camera.h"
#include "../base/WinApp.h"

#include "../2d/SpriteCommon.h"
#include "../base/TextureManager.h"

#include "../3d/Model.h"
#include "../3d/ModelManager.h"
#include "../3d/Object3d.h"
#include "../3d/Object3dCommon.h"

#include "../3d/Skybox/SkyBoxCommon.h"
#include "../3d/Skybox/Skybox.h"

#include "../3d/ParticleEmitter.h"
#include "../3d/ParticleManager.h"

#include "../io/Input.h"

#include "math.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene() = default;

void TitleScene::Initialize()
{
    TextureManager::getInstance()->LoadTexture("resources/rostock_laage_airport_4k.dds");

    skydox = std::make_unique<Skybox>();
    skydox->Initialize("resources/rostock_laage_airport_4k.dds");
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{

    Input* input = BaseScene::GetInput();
    Camera* camera = BaseScene::GetCamera();

    skydox->SetCamera(camera);

    if (input->TriggerKey(DIK_0)) {
        auto scene = std::make_unique<GamePlayScene>();
        scene->SetInput(BaseScene::GetInput());

        SceneManager::GetInstance()->SetNextScene(std::move(scene));
    }

    skydox->Update();
}

void TitleScene::Draw()
{
    Object3dCommon::GetInstance()->PrepareObjectDraw();

    SkyBoxCommon::GetInstance()->PrepareObjectDraw();
    skydox->Draw();

    SpriteCommon::GetInstance()->PrepareSpriteDraw();
}
