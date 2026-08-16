#pragma once
#include "../../Engine/3d/Model.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../Particle/LaserParticle.h"
#include <memory>

#include "../OnCollison/Collider.h"

class Camera;
class EnemyManager;

class PlayerBullet : public Collider {
public:
    void Initialize(Camera* camera, const Vector3& position, const Vector3& rotation);

    void Update(float deltaTime);

    void Draw();

public:
    // Get関数
    bool IsDead() const { return isDead_; }
    bool IsChargBullet() const { return isChargeBullet; }

    AABB GetAABB() const override;
    CollisionGroup GetCollisionGroup() const override { return CollisionGroup::kPlayerBullet; }
    void OnCollision(Collider* other) override;
    int GetDamage() const override;

    // set
    void SetTarget(uint32_t targetId, EnemyManager* manager)
    {
        targetId_ = targetId;
        enemyManager_ = manager;
        isChargeBullet = true;

    } // 対象をセット

private:
    Transform transform_ = { 0.0f, 0.0f, 0.0f }; // 座標

    // 当たり判定
    float size = 0.5f; // OBBに移植後は知らん。

    Vector3 velocity_ = { 0.0f, 0.0f, 0.0f }; // 移動ベクトル
    float speed_ = 50.0f; // 弾速
    float deathTimer_ = 3.0f; // 弾の寿命（秒）
    bool isDead_ = false;

    float particleTimer_ = 0.0f; // 経過時間タイマー
    const float kParticleInterval_ = 0.025f; // パーティクル発生間隔

    uint32_t targetId_ = 0;
    EnemyManager* enemyManager_ = nullptr;
    float homingStrength_ = 0.2f; // 追ビ性能(ほぼ必中で良い)
    bool isChargeBullet = false;
    int Dameg = 1;
    int ChageDameg = 5;

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    std::unique_ptr<LaserParticle> laserParticle_;
};
