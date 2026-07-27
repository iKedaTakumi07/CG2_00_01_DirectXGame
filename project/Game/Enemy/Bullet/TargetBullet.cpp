#include "TargetBullet.h"

#include "../../../Engine/3d/Camera.h"
#include "../../../Engine/3d/ModelManager.h"
#include "../../../Engine/3d/Object3d.h"
#include "../../../Engine/base/Math.h"
#include "../../../Engine/base/TextureManager.h"
#include "../../../Engine/io/Input.h"

#include "../../Particle/LaserParticle.h"

void TargetBullet::Initialize(Camera* camera, Vector3 pos, const Vector3& rotation)
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
    transform_.translate = pos;
    transform_.rotate = rotation;

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);

    laserParticle_ = std::make_unique<LaserParticle>();
    laserParticle_->Initialize();
}

void TargetBullet::Update(float deltaTime)
{
    // 弾の移動
    velocity_ += acceleration_;

    float currentSpeed = sqrtf(velocity_.x * velocity_.x + velocity_.y * velocity_.y + velocity_.z * velocity_.z);

    // 弾の速さが最高速度を超えていたら、最高速度に制限する
    float totalMaxSpeed = maxSpeed; // 合計の限界値を出す
    if (currentSpeed >= totalMaxSpeed) {
        // 現在の進行方向（長さ1）を計算し、それに最高速度を掛ける
        Vector3 currentDir = Normalize(velocity_);
        velocity_ = currentDir * totalMaxSpeed;
    }

    transform_.translate += velocity_;

    deathTimer_ -= deltaTime;
    if (deathTimer_ <= 0.0f) {
        isDead_ = true;
    }

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);
    object3d->Update();

    particleTimer_ += deltaTime;
}

void TargetBullet::Draw()
{
    object3d->Draw();

    if (particleTimer_ >= kParticleInterval_) {
        // リセット
        particleTimer_ = 0.0f;
        laserParticle_->NewParticle(transform_);
    }
}

void TargetBullet::SetTargetPosition(Vector3 Pos)
{
    // 狙う場所を設定
    targetPos_ = Pos;

    // ターゲットへのベクトル ＝ 目的地の座標 － 現在の座標
    Vector3 direction;
    direction.x = targetPos_.x - transform_.translate.x;
    direction.y = targetPos_.y - transform_.translate.y;
    direction.z = targetPos_.z - transform_.translate.z;

    // directionを「長さが1のベクトル（正規化ベクトル）」にする
    direction = Normalize(direction);

    velocity_ = { 0.0f, 0.0f, 0.0f };

    // 加速度の「大きさ」を決め、それにターゲットの方向を掛け合わせる
    acceleration_ = direction * accelerationScalar;
}
