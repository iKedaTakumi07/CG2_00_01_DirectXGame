#include "TitleScene.h"
#include "GamePlayScene.h"
#include "SceneManager.h"

#include "Input.h"
#include "Model.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"

#include "Game.h"
#include "TextureManager.h"

void TitleScene::Initialize()
{
    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");

    ModelManager::GetInstance()->LoadModel("Plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");

    object3d = new Object3d();
    object3d->Initialize(/*object3dCommon, winapp*/);

    model = new Model();
    model->Initialize(/*modelCommon,*/ "resources", "plane.obj");
    object3d->SetModel(model);

    object3d2 = new Object3d();
    object3d2->Initialize(/*object3dCommon, winApp*/);

    model2 = new Model();
    model2->Initialize(/*modelCommon,*/ "resources", "axis.obj");
    object3d2->SetModel(model2);
}

void TitleScene::Finalize()
{
    delete object3d;
    delete object3d2;
    delete model;
    delete model2;
}

void TitleScene::Update()
{
  
    Input* input = BaseScene::GetInput();

    if (input->TriggerKey(DIK_0)) {
        auto scene = std::make_unique<GamePlayScene>();
        scene->SetInput(BaseScene::GetInput());

        SceneManager::GetInstance()->SetNextScene(std::move(scene));
    }

    Vector3 rotate = object3d->GetRotate();
    rotate.x += 0.1f;
    rotate.y += 0.1f;
    object3d->SetRotate(rotate);
    object3d->Update();

    object3d2->Update();
    Vector3 rotate2 = object3d2->GetRotate();
    rotate2.x += -0.1f;
    rotate2.y += -0.1f;
    object3d2->SetRotate(rotate2);
}

void TitleScene::Draw()
{
    Object3dCommon::GetInstance()->PrepareObjectDraw();

    // モデルデータ
    object3d->Draw();
    object3d2->Draw();
}
