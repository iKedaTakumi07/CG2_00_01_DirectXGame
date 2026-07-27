#pragma once
#include "../../Engine/3d/Model.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../Particle/LaserParticle.h"
#include <memory>

class Camera;
class PlayerBullet {
public:
    void Initialize(Camera* camera, const Vector3& position, const Vector3& rotation);

    void Update(float deltaTime);

    void Draw();

    // set

    bool IsDead() const { return isDead_; }

private:
    Transform transform_ = { 0.0f, 0.0f, 0.0f }; // 座標

    Vector3 velocity_ = { 0.0f, 0.0f, 0.0f }; // 移動ベクトル
    float speed_ = 50.0f; // 弾速
    float deathTimer_ = 3.0f; // 弾の寿命（秒）
    bool isDead_ = false;

    float particleTimer_ = 0.0f; // 経過時間タイマー
    const float kParticleInterval_ = 0.025f; // パーティクル発生間隔

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    std::unique_ptr<LaserParticle> laserParticle_;
};
