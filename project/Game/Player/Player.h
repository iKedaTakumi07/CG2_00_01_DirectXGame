#pragma once
#include "../../Engine/3d/Camera.h"
#include "../../Engine/3d/Model.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include <memory>
#include <list>

#include "PlayerBullet.h"
class Player {
public:
    void Initialize(Camera* camera);

    void Update();

    void Draw();

    // set
private:
    // 更新系列
    void MoveUpdate();
    // 弾の制御
    void BulletUpdate();

private:
    Transform transform_ = { 0.0f, 0.0f, 0.0f }; // 座標

    // 移動速度
    const float kCharacterSpeed = 0.2f;
    // 移動限界座標
    const float kMoveLimitX = 10.0f;
    const float kMoveLimitY = 7.5f;
    // 機体の傾き
    float rollFactor = 0.3f;
    float shiftRollFactor = 0.8f;

    // 弾の詳細設定(チャージショット一連の操作、チュートリアルを作成後作成)
    const float kCoolTime = 0.20f;
    float coolTime = 0.0f;

    // 3dモデル
    std::unique_ptr<Model> model;
    std::unique_ptr<Object3d> object3d;

    // カメラ
    Camera* camera_;

    // 弾
    std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
};
