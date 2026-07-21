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

    // 高速旋回
    bool isShift = Input::getInstance()->PushKey(DIK_LSHIFT);

    float roll = rollFactor;
    Vector3 speed = Vector3(kCharacterSpeed, kCharacterSpeed, 0.0f);

    if (isShift) {
        roll = shiftRollFactor; // 旋回
        speed.x = kCharacterSpeed * 1.5f; // 速度上昇
    }

    // 正規化
    float length = std::sqrt(move.x * move.x + move.y * move.y);
    if (length > 0.0f) {
        move.x = (move.x / length) * speed.x;
        move.y = (move.y / length) * speed.y;
    }

    transform_.translate += move;

    transform_.translate.x = std::clamp(transform_.translate.x, -kMoveLimitX, kMoveLimitX);
    transform_.translate.y = std::clamp(transform_.translate.y, -kMoveLimitY, kMoveLimitY);

    const float kTargetRoll = -move.x * roll;
    const float kTargetYRoll = -move.y * rollFactor; // 固定y

    // 高速旋回しているか?
    float lerpRate = isShift ? 0.2f : 0.1f;

    transform_.rotate.z += (kTargetRoll - transform_.rotate.z) * lerpRate;
    transform_.rotate.x += (kTargetYRoll - transform_.rotate.x) * 0.1f;
}

void Player::BulletUpdate()
{
    float deltaTime = SceneManager::GetInstance()->GetDeltaTime();

    if (coolTime <= 0.0f) {
        if (Input::getInstance()->PushKey(DIK_SPACE)) {
            auto playerbullet = std::make_unique<PlayerBullet>();
            playerbullet->Initialize(camera_, transform_.translate, transform_.rotate);

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
