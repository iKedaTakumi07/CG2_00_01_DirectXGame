#pragma once
#include "../../OnCollison/Collider.h"
#include "../base/baseBossEnemy.h "
#include <memory>

class Player;
class Model;
class Object3d;

class FourEyesBoss : public baseBossEnemy {
public:
    void Initialize(Vector3 pos) override;
    void Update() override;
    void Draw() override;
    void SpriteDraw() override;

    float GetHpRate() const override { return static_cast<float>(currentHp_) / maxHp_; }
    int GetCurrentPhase() const override { return currentPhase_; }

    AllAABB GetAllAABB() const override;
    CollisionGroup GetCollisionGroup() const override { return CollisionGroup::kEnenmy; }
    void OnCollision(Collider* other) override;
    int GetDamage() const override { return dameg_; }
    bool GetIsAvile_() override { return isAvile_; }
    Vector3 GetTranslate() override { return transform_.translate; }

    void StartAppearance() override;
    void UpdateAppearance(float deltaTime) override;
    bool IsAppearing() const override { return isAppearing_; }

private:
    void FireFourWayBullets();

private:
    Camera* camera_ = nullptr; // カメラ(ポインタ)
    Player* player_ = nullptr;

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    float appearanceTimer_ = 0.0f;
    const float kAppearanceDuration = 3.0f;

    Transform transform_ = { 0.0f }; // 座標系
    Vector3 centerPos_ = { 0.0f }; // 中心位置

    int dameg_ = 5;
    bool isAvile_ = true; // 存在しているか
    bool isDead_ = false; // 死んでいるか

    // 発射位置(各頂点の中心位置)
    std::array<Vector3, 4> muzzleOffsets_ = {
        Vector3 { -6.0f, 3.0f, 0.0f },
        Vector3 { 6.0f, 3.0f, 0.0f },
        Vector3 { -6.0f, -3.0f, 0.0f },
        Vector3 { 6.0f, -3.0f, 0.0f }
    };
};
