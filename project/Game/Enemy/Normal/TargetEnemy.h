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

    // set関数
    void SetTargetPlayer(Player* target) override { player_ = target; }

private:
    void BulletUpdate();

private:
    Camera* camera_ = nullptr; // カメラ(ポインタ)
    Player* player_ = nullptr;

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    Transform transform_; // 座標系

    int health_; // 体力(jsonで設定予定)

    float interval; // 弾を発射する間隔
    static inline const float maxInterval = 2.0f; // 間隔

    // 削除予定 //
    float move = 1.0f / 60.0f;
    // 移動地点はjson形式予定。 //

    bool isAvile_ = true; // 存在しているか
    bool isDead_ = false; // 死んでいるか
};
