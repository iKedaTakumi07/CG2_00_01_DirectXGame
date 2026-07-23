#pragma once
#include "../base/baseEnemy.h"
#include <memory>

#include "../../../Engine/3d/Object3d.h"
#include "../../../Engine/base/Math.h"
class Model;

class TargetEnemy : public baseEnemy {
public:
    void Initialize(Camera* camera, Vector3 posh) override;

    void Update() override;

    void Draw() override;

private:
    Camera* camera_ = nullptr; // カメラ(ポインタ)

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    Transform transform_; // 座標系

    int health_; // 体力(jsonで設定予定)

    // 削除予定 //
    float move = 1.0f / 60.0f;
    // //

    bool isAvile_ = true; // 存在しているか
    bool isDead_ = false; // 死んでいるか
};
