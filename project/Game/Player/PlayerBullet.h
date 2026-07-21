#pragma once
#include "../../Engine/3d/Model.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include <memory>

class Camera;
class PlayerBullet {
public:
    void Initialize(Camera* camera, const Vector3& position);

    void Update(float deltaTime);

    void Draw();

    // set

    bool IsDead() const { return isDead_; }

private:
    Transform transform_ = { 0.0f, 0.0f, 0.0f }; // 座標

    float vectorZ = 10.0f;
    float deathTimer_ = 3.0f; // 弾の寿命（秒）
    bool isDead_ = false;

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;
};
