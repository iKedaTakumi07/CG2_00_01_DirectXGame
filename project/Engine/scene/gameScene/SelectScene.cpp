#include "SelectScene.h"
#include "../SceneManager.h"

#include "../../../Game/stage/stageDataLoad.h"
#include "../../2d/SpriteCommon.h"
#include "../../3d/CPUParticle/CPUParticleManager.h"
#include "../../3d/Camera.h"
#include "../../3d/CameraManager.h"
#include "../../3d/Object3dCommon.h"
#include "../../3d/Skybox/SkyBoxCommon.h"
#include "../../base/TextureManager.h"
#include "../../io/Input.h"

SelectScene::SelectScene()
{
}

SelectScene::~SelectScene() = default;

void SelectScene::Finalize()
{
}

void SelectScene::Initialize()
{
    Camera* mainCamera = CameraManager::GetInstance()->CreateCamera("PlayMain");
    mainCamera->SetTranslate({ 0.0f, 2.0f, -15.0f });

    CameraManager::GetInstance()->SetActiveCamera("PlayMain");

    TextureManager::getInstance()->LoadTexture("resources/selectUI/stage1UI.png");
    TextureManager::getInstance()->LoadTexture("resources/selectUI/stage2UI.png");

    SatgeUI1 = std::make_unique<Sprite>();
    SatgeUI1->Initialize("resources/selectUI/stage1UI.png");
    SatgeUI1->SetPosition(Vector2(400.0f, 280.0f));

    SatgeUI2 = std::make_unique<Sprite>();
    SatgeUI2->Initialize("resources/selectUI/stage2UI.png");
    SatgeUI2->SetPosition(Vector2(400.0f, 280.0f));

    stageNumber = 1;
}

void SelectScene::Update()
{
    auto* input = Input::getInstance();

    if (!selectStop) {
        if (input->TriggerKey(DIK_D) || input->TriggerKey(DIK_RIGHTARROW)) {
            if (stageNumber < MaxStageNumber) {
                stageNumber++;
            }
        }
        if (input->TriggerKey(DIK_A) || input->TriggerKey(DIK_LEFTARROW)) {
            if (stageNumber > MinStageNumber) {
                stageNumber--;
            }
        }
    }

    if (input->TriggerKey(DIK_SPACE)) {
        selectStop = true;
        stageDataLoad::GetInstance()->SetStage(stageNumber);
        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }

    SatgeUI1->Update();
    SatgeUI2->Update();
}

void SelectScene::Draw()
{
    Object3dCommon::GetInstance()->PrepareObjectDraw();
    //
    // モデルデータ
    //

    SkyBoxCommon::GetInstance()->PrepareObjectDraw();
    // skydox->Draw();

    //
    // 2d/スプライト
    //
    SpriteCommon::GetInstance()->PrepareSpriteDraw();
    if (stageNumber == 1) {
        SatgeUI1->Draw();
    } else if (stageNumber == 2) {
        SatgeUI2->Draw();
    }

    CPUParticleManager::getInstance()->Draw();
}
