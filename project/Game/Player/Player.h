#pragma once
#include "../../Engine/3d/Camera.h"
#include "../../Engine/3d/Model.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include <list>
#include <memory>

#include "../OnCollison/Collider.h"

#include "PlayerBullet.h"

class Player : public Collider {
public:
    void Initialize(Camera* camera);

    void Update();

    void Draw();

public:
    // Get関数
    Vector3 GetTranslate() const { return basetransform_.translate; } // 座標の取得
    Transform GetTransform() const { return basetransform_; } // 回転角含む
    const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() const { return playerBullets_; } // 弾の入手

    AABB GetAABB() const override;
    CollisionGroup GetCollisionGroup() const override { return CollisionGroup::kPlayer; }
    void OnCollision(Collider* other) override;
    int GetDamage() const override { return dameg_; }

    // set

private:
    // 更新系列
    void MoveUpdate();
    void HoverUpdate();
    // 弾の制御
    void BulletUpdate();

private:
    Transform transform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }; // 座標
    Transform basetransform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }; // 揺れ成分を含まない座標他

    // 移動速度
    Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };

    // 当たり判定
    float size = 2.0f; // OBBに移植後は知らん。

    // 体力
    int hp_ = 100; // 現体力
    int Maxhp_ = 100; // 最大体力

    // 移動系パラメータ
    const float kCharacterSpeed = 0.4f; // 最高速度
    const float kAcceleration = 0.02f; // 加速度
    const float shiftUpSpeed = 1.25f; // シフト(高速旋回)乗算倍率
    const float kFriction = 0.87f; // 摩擦抵抗

    // 移動限界座標
    const float kMoveLimitX = 8.0f;
    const float kMoveLimitY = 7.0f;

    // 機体の傾き
    float rollFactor = 0.8f;
    float shiftRollFactor = 1.4f;

    // 静止時の揺れ
    const float kHoverSpeed = 2.5f; // 浮遊の速さ（周波数）
    const float kHoverAmount = 0.015f; // 浮遊の揺れ幅（上下移動量）
    const float kSwaySpeed = 4.0f; // 揺れる速さ（周波数）
    const float kSwayAmountZ = 0.025f; // Roll（左右の傾き）の揺れ幅
    const float kSwayAmountX = 0.015f; // Pitch（前後の傾き）の揺れ幅
    float idleTimer_ = 0.0f; // 揺れタイマー

    // 弾の詳細設定(チャージショット一連の操作、チュートリアルを作成後作成)
    const float kCoolTime = 0.20f;
    float coolTime = 0.0f;
    int dameg_ = 10;
    int chargeDameg_ = 15;

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    // カメラ
    Camera* camera_;

    // 弾
    std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
};
