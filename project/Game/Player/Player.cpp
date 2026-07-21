#define NOMINMAX

#include "Player.h"
#include <algorithm>
#include <cmath>

#include "../../Engine/3d/ModelManager.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../../Engine/base/TextureManager.h"
#include "../../Engine/io/Input.h"
#include "../../Engine/scene/SceneManager.h"

#include "PlayerBullet.h"
#include <utility>

void Player::Initialize(Camera* camera)
{
    TextureManager::getInstance()->LoadTexture("resources/player/1x1white.png");
    ModelManager::GetInstance()->LoadModel("player/Player.obj");

    camera_ = camera;

    object3d = std::make_unique<Object3d>();
    object3d->Initialize();
    object3d->SetCamera(camera);

    model = std::make_unique<Model>();
    model->Initialize("resources/player", "Player.obj");
    // model->SetEvnTexturefilePath(skydox->GetTextureFilePath()); // 反射が必要なら
    object3d->SetModel(model.get());
}

void Player::Update()
{
    MoveUpdate();
    BulletUpdate();

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);

    object3d->Update();
    object3d->DrawImGui("Player");
}

void Player::Draw()
{
    for (auto& bullet_ : playerBullets_) {
        bullet_->Draw();
    }

    object3d->Draw();
}

void Player::MoveUpdate()
{
    Vector3 move = { 0, 0, 0 };

    // 押した方向でベクトル変更
    if (Input::getInstance()->PushKey(DIK_A)) {
        move.x -= 1.0f;
    }
    if (Input::getInstance()->PushKey(DIK_D)) {
        move.x += 1.0f;
    }
    if (Input::getInstance()->PushKey(DIK_W)) {
        move.y += 1.0f;
    }
    if (Input::getInstance()->PushKey(DIK_S)) {
        move.y -= 1.0f;
    }

    // 正規化
    float length = std::sqrt(move.x * move.x + move.y * move.y);
    if (length > 0.0f) {
        move.x = (move.x / length) * kCharacterSpeed;
        move.y = (move.y / length) * kCharacterSpeed;
    }

    transform_.translate += move;

    transform_.translate.x = std::clamp(transform_.translate.x, -kMoveLimitX, kMoveLimitX);
    transform_.translate.y = std::clamp(transform_.translate.y, -kMoveLimitY, kMoveLimitY);

    const float kTargetRoll = -move.x * 0.3f;
    transform_.rotate.z += (kTargetRoll - transform_.rotate.z) * 0.1f;
}

void Player::BulletUpdate()
{
    float deltaTime = SceneManager::GetInstance()->GetDeltaTime();

    if (coolTime <= 0.0f) {
        if (Input::getInstance()->PushKey(DIK_SPACE)) {
            auto playerbullet = std::make_unique<PlayerBullet>();
            playerbullet->Initialize(camera_, transform_.translate);

            // リストに挿入
            playerBullets_.push_back(std::move(playerbullet));

            coolTime = kCoolTime;
        }
    } else {
        coolTime -= deltaTime;
    }

    for (auto& bullet_ : playerBullets_) {
        bullet_->Update(deltaTime);
    }

    std::erase_if(playerBullets_, [](const std::unique_ptr<PlayerBullet>& bullet) {
        return bullet->IsDead();
    });
}
