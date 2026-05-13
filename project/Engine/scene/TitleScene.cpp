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
    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");
    TextureManager::getInstance()->LoadTexture("resources/grass.png");

    ModelManager::GetInstance()->LoadModel("axis.obj");
    ModelManager::GetInstance()->LoadModel("terrain.obj");

    skydox = std::make_unique<Skybox>();
    skydox->Initialize("resources/rostock_laage_airport_4k.dds");

    object3d = std::make_unique<Object3d>();
    object3d->Initialize();
    object3d->SetCamera(BaseScene::GetCamera());

    model = std::make_unique<Model>();
    model->Initialize("resources", "terrain.obj");
    model->SetEvnTexturefilePath(skydox->GetTextureFilePath());
    object3d->SetModel(model.get());
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{

    Input* input = BaseScene::GetInput();
    Camera* camera = BaseScene::GetCamera();

    skydox->SetCamera(camera);

    if (input->TriggerKey(DIK_F1)) {
        auto scene = std::make_unique<GamePlayScene>();
        scene->SetInput(BaseScene::GetInput());
        scene->SetCamera(BaseScene::GetCamera());

        SceneManager::GetInstance()->SetNextScene(std::move(scene));
    }

    skydox->Update();

    object3d->Update();

    // IMGUI
    object3d->DrawImGui();
}

void TitleScene::Draw()
{

    //
    // モデルデータ
    //
    Object3dCommon::GetInstance()->PrepareObjectDraw();

    object3d->Draw();

    SkyBoxCommon::GetInstance()->PrepareObjectDraw();
    skydox->Draw();

    SpriteCommon::GetInstance()->PrepareSpriteDraw();
}
