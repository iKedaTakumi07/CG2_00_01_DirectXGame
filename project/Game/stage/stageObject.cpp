#include "stageObject.h"
#include "../../Engine/3d/CameraManager.h"
#include "../../Engine/3d/ModelManager.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../../Engine/base/TextureManager.h"

void stageObject::Initialize()
{
    TextureManager::getInstance()->LoadTexture("resources/stage/uvChecker.png");
    ModelManager::GetInstance()->LoadModel("stage/stageGraunod.obj");

    grauond3d = std::make_unique<Object3d>();
    grauond3d->Initialize();

    grauondModel = std::make_unique<Model>();
    grauondModel->Initialize("resources/stage", "stageGraunod.obj");
    grauond3d->SetModel(grauondModel.get());
    grauond3d->SetScale(grauondtransform_.scale);
}

void stageObject::Update()
{
    grauond3d->SetTranslate(grauondtransform_.translate);
    grauond3d->SetRotate(grauondtransform_.rotate);

    grauond3d->Update();
}

void stageObject::Draw()
{
    grauond3d->Draw();
}
