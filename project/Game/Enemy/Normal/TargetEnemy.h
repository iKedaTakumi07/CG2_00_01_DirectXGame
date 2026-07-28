#pragma once
#include "../base/baseEnemy.h"
#include <memory>

#include "../../../Engine/3d/Object3d.h"
#include "../../../Engine/base/Math.h"

#include "../../Player/Player.h"

class Model;

class TargetEnemy : public baseEnemy {
public:
    void Initialize(Camera* camera, Vector3 posh) override;

    void Update() override;

    void Draw() override;

public:
    // Get関数
    AABB GetAABB() const override;
    CollisionGroup GetCollisionGroup() const override { return CollisionGroup::kEnenmy; }
    void OnCollision(Collider* other) override;
    int GetDamage() const override { return dameg_; }

    // set関数
    void SetTargetPlayer(Player* target) override { player_ = target; }
    void SetIsDead(bool num) { isDead_ = num; }

private:
    void BulletUpdate();

private:
    Camera* camera_ = nullptr; // カメラ(ポインタ)
    Player* player_ = nullptr;

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    // 当たり判定
    float size = 2.0f; // OBBに移植後は知らん。

    Transform transform_; // 座標系

    int health_; // 体力(jsonで設定予定)
    int dameg_ = 5;

    float interval; // 弾を発射する間隔
    static inline const float maxInterval = 2.0f; // 間隔

    // 削除予定 //
    float move = 1.0f / 60.0f;
    // 移動地点はjson形式予定。 //

    bool isAvile_ = true; // 存在しているか
    bool isDead_ = false; // 死んでいるか
};
