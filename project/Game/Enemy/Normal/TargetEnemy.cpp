#include "TargetEnemy.h"

#include "../../../Engine/3d/Model.h"
#include "../../../Engine/3d/ModelManager.h"

#include "../../../Engine/base/TextureManager.h"

void TargetEnemy::Initialize(Camera* camera, Vector3 pos)
{
    TextureManager::getInstance()->LoadTexture("resources/test/uvChecker.png");
    ModelManager::GetInstance()->LoadModel("test/test.obj");

    object3d = std::make_unique<Object3d>();
    object3d->Initialize();
    object3d->SetCamera(camera);

    model = std::make_unique<Model>();
    model->Initialize("resources/test", "test.obj");
    // model->SetEvnTexturefilePath(skydox->GetTextureFilePath()); // 反射が必要なら
    object3d->SetModel(model.get());

    camera_ = camera;

    transform_.scale = { 1.0f, 1.0f, 1.0f };
    transform_.rotate = { 0.0f, 0.0f, 0.0f };
    transform_.translate = pos;
}

void TargetEnemy::Update()
{
    transform_.translate.x += move;

    if (transform_.translate.x + move >= 10.0f || transform_.translate.x + move <= -10.0f) {
        move = -move;
    }

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);
    object3d->Update();
}

void TargetEnemy::Draw()
{
    object3d->Draw();
}
