#include "PlayerBullet.h"

#include "../../Engine/3d/Camera.h"
#include "../../Engine/3d/ModelManager.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../../Engine/base/TextureManager.h"
#include "../../Engine/io/Input.h"

void PlayerBullet::Initialize(Camera* camera, const Vector3& position, const Vector3& rotation)
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

    // 座標セット
    transform_.translate = position;
    transform_.rotate = rotation;

    velocity_.x = -std::sin(transform_.rotate.z) * speed_;
    velocity_.y = -std::sin(transform_.rotate.x) * speed_;
    velocity_.z = std::cos(transform_.rotate.z) * speed_;

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);
}

void PlayerBullet::Update(float deltaTime)
{
    // 弾の移動
    transform_.translate.x += velocity_.x * deltaTime;
    transform_.translate.y += velocity_.y * deltaTime;
    transform_.translate.z += velocity_.z * deltaTime;

    deathTimer_ -= deltaTime;
    if (deathTimer_ <= 0.0f) {
        isDead_ = true;
    }

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);
    object3d->Update();
}

void PlayerBullet::Draw()
{
    object3d->Draw();
}
