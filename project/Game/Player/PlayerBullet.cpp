#include "PlayerBullet.h"

#include "../../Engine/3d/Camera.h"
#include "../../Engine/3d/ModelManager.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../../Engine/base/TextureManager.h"
#include "../../Engine/io/Input.h"

#include "../Particle/LaserParticle.h"

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

    // 進行方向ベクトルからモデルの向きを計算
    transform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
    float hypotXZ = std::hypot(velocity_.x, velocity_.z);
    transform_.rotate.x = std::atan2(-velocity_.y, hypotXZ);
    transform_.rotate.z = rotation.z;

    Vector3 renderRotate = transform_.rotate;
    renderRotate.x *= -1.0f;

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(renderRotate);

    laserParticle_ = std::make_unique<LaserParticle>();
    laserParticle_->Initialize();
    laserParticle_->SetStartColor(Vector4(0.5f, 1.0f, 0.5f, 1.0f));
    laserParticle_->SetEndColor(Vector4(0.5f, 1.0f, 0.5f, 0.0f));
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

    transform_.rotate.y = std::atan2(velocity_.x, velocity_.z);
    float hypotXZ = std::hypot(velocity_.x, velocity_.z);
    transform_.rotate.x = std::atan2(-velocity_.y, hypotXZ);

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);
    object3d->Update();

    particleTimer_ += deltaTime;
}

void PlayerBullet::Draw()
{
    object3d->Draw();

    if (particleTimer_ >= kParticleInterval_) {
        // リセット
        particleTimer_ = 0.0f;
        laserParticle_->NewParticle(transform_);
    }
}

AABB PlayerBullet::GetAABB() const
{
    AABB aabb;

    aabb.min = { transform_.translate.x - size, transform_.translate.y - size, transform_.translate.z - size };
    aabb.max = { transform_.translate.x + size, transform_.translate.y + size, transform_.translate.z + size };
    return aabb;
}

void PlayerBullet::OnCollision(Collider* other)
{
    // 当たったもの次第で分岐
    if (other->GetCollisionGroup() == CollisionGroup::kEnemyBullet || other->GetCollisionGroup() == CollisionGroup::kEnenmy) {
        // 消滅
        isDead_ = true;

        // チャージショット実装するなら消滅しない例外が必要かも
    }
}
