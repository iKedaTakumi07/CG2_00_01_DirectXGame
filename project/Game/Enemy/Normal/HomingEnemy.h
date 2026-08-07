#pragma once
#include "../base/baseEnemy.h"
#include <memory>

#include "../../../Engine/3d/Object3d.h"
#include "../../../Engine/base/Math.h"

class Player;
class Model;

class HomingEnemy : public baseEnemy {
public:
    void Initialize(Vector3 posh) override;

    void Update() override;

    void Draw() override;

public:
    // Get関数
    AABB GetAABB() const override;
    CollisionGroup GetCollisionGroup() const override { return CollisionGroup::kEnenmy; }
    void OnCollision(Collider* other) override;
    int GetDamage() const override { return dameg_; }
    bool GetIsAvile_() override { return isAvile_; }

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
    float size = 0.5f; // OBBに移植後は知らん。

    Transform transform_ = { 0.0f }; // 座標系

    int health_ = 5; // 体力(jsonで設定予定)
    int dameg_ = 3;

    float interval = 3.0f; // 弾を発射する間隔
    static inline const float maxInterval = 3.0f; // 間隔

    // 削除予定 //
    float move = 1.0f / 60.0f;
    // 移動地点はjson形式予定。 //

    bool isAvile_ = true; // 存在しているか
    bool isDead_ = false; // 死んでいるか
};
