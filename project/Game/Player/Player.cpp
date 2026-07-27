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
    float deltaTime = SceneManager::GetInstance()->GetDeltaTime();
    idleTimer_ += deltaTime;

    Vector3 inputDir = { 0, 0, 0 };

    // 押した方向でベクトル変更
    if (Input::getInstance()->PushKey(DIK_A)) {
        inputDir.x -= 1.0f;
    }
    if (Input::getInstance()->PushKey(DIK_D)) {
        inputDir.x += 1.0f;
    }
    if (Input::getInstance()->PushKey(DIK_W)) {
        inputDir.y += 1.0f;
    }
    if (Input::getInstance()->PushKey(DIK_S)) {
        inputDir.y -= 1.0f;
    }

    // 正規化
    float length = std::sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
    if (length > 0.0f) {
        inputDir.x /= length;
        inputDir.y /= length;
    }

    // 高速旋回
    bool isShift = Input::getInstance()->PushKey(DIK_LSHIFT) || Input::getInstance()->PushKey(DIK_RSHIFT);

    float currentAccel = isShift ? kAcceleration * shiftUpSpeed : kAcceleration; // 加速度
    float currentMaxSpeed = isShift ? kCharacterSpeed * shiftUpSpeed : kCharacterSpeed; // 速度

    // 指定方向に加速
    velocity_.x += inputDir.x * currentAccel;
    velocity_.y += inputDir.y * currentAccel;

    // 摩擦による減速
    velocity_.x *= kFriction;
    velocity_.y *= kFriction;

    // 最高速の制限
    float speed = velocity_.x * velocity_.x + velocity_.y * velocity_.y;
    if (speed > currentMaxSpeed * currentMaxSpeed) {
        float currentSpeed = std::sqrt(speed);
        velocity_.x = (velocity_.x / currentSpeed) * currentMaxSpeed;
        velocity_.y = (velocity_.y / currentSpeed) * currentMaxSpeed;
    }

    // 座標に代入
    basetransform_.translate.x += velocity_.x;
    basetransform_.translate.y += velocity_.y;

    basetransform_.translate.x = std::clamp(basetransform_.translate.x, -kMoveLimitX, kMoveLimitX);
    basetransform_.translate.y = std::clamp(basetransform_.translate.y, -kMoveLimitY, kMoveLimitY);

    // 高速旋回しているか?
    float lerpRate = isShift ? shiftRollFactor : rollFactor;

    // 揺れを含まない回転角
    const float kTargetRoll = -(velocity_.x / currentMaxSpeed) * lerpRate;
    const float kTargetYRoll = -(velocity_.y / currentMaxSpeed) * lerpRate;

    // 補間の速度
    float lerpSpeed = isShift ? 15.0f : 8.0f;
    float t = 1.0f - std::exp(-lerpSpeed * deltaTime);

    basetransform_.rotate.y -= (kTargetRoll + basetransform_.rotate.y) * t;
    basetransform_.rotate.x += (kTargetYRoll - basetransform_.rotate.x) * t;

    // 揺れの計算
    HoverUpdate();
}

void Player::HoverUpdate()
{
    // 揺れを含む座標系
    float hoverY = std::sin(idleTimer_ * kHoverSpeed) * kHoverAmount;
    transform_.translate = basetransform_.translate;
    transform_.translate.y += hoverY;

    // 揺れ込みの回転角
    float swayZ = std::sin(idleTimer_ * kSwaySpeed) * kSwayAmountZ;
    float swayX = std::cos(idleTimer_ * kSwaySpeed * 0.7f) * kSwayAmountX;

    transform_.rotate.y = basetransform_.rotate.y + swayZ;
    transform_.rotate.x = basetransform_.rotate.x + swayX;
}

void Player::BulletUpdate()
{
    float deltaTime = SceneManager::GetInstance()->GetDeltaTime();

    if (coolTime <= 0.0f) {
        if (Input::getInstance()->PushKey(DIK_SPACE)) {
            auto playerbullet = std::make_unique<PlayerBullet>();
            playerbullet->Initialize(camera_, basetransform_.translate, basetransform_.rotate);

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
