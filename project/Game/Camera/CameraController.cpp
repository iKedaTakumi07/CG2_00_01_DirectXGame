#include "CameraController.h"
#include "../../Engine/3d/Camera.h"
#include "../../Engine/scene/SceneManager.h"
#include "../Player/Player.h"
#include <algorithm>
#include <cmath>

void CameraController::Initialize(Camera* camera, const Player* player)
{
    camera_ = camera;
    player_ = player;

    if (camera_) {
        defaultCameraPos_ = camera_->GetTranslate();
    }
}

void CameraController::Update()
{
    float deltaTime_ = SceneManager::GetInstance()->GetDeltaTime();

    // カメラ又はプレイヤーがいないなら早期リターン(ボス実装するなら変更するかも)
    if (!camera_ || !player_) {
        return;
    }

    // プレイヤーの座標を入手
    Vector3 playerPos = player_->GetTranslate();
    float pLimitX = player_->GetLimitX();
    float pLimitY = player_->GetLimitY();

    // カメラが動くか判別
    float deadZoneX = 0.0f;
    float deadZoneY = 0.0f;

    if (playerPos.x > baseDeadZoneX_) {
        deadZoneX = playerPos.x - baseDeadZoneX_;
    } else if (playerPos.x < -baseDeadZoneX_) {
        deadZoneX = playerPos.x + baseDeadZoneX_;
    }
    if (playerPos.y > baseDeadZoneY_) {
        deadZoneY = playerPos.y - baseDeadZoneY_;
    } else if (playerPos.y < -baseDeadZoneY_) {
        deadZoneY = playerPos.y + baseDeadZoneY_;
    }

    float rangeX = deadZoneX / (pLimitX - baseDeadZoneX_);
    float rangeY = deadZoneY / (pLimitY - baseDeadZoneY_);

    rangeX = std::clamp(rangeX, -1.0f, 1.0f);
    rangeY = std::clamp(rangeY, -1.0f, 1.0f);

    // カメラの移動量を計算
    Vector3 targetCameraPos = defaultCameraPos_;
    Vector3 currentPos = camera_->GetTranslate();

    targetCameraPos.x += rangeX * maxShiftX_;
    targetCameraPos.y += rangeY * maxShiftY_;

    targetCameraPos.z = playerPos.z + defaultOffsetZ_;

    float t = 1.0f - std::exp(-followSpeed_ * deltaTime_);

    Vector3 newPos;
    newPos.x = currentPos.x + (targetCameraPos.x - currentPos.x) * t;
    newPos.y = currentPos.y + (targetCameraPos.y - currentPos.y) * t;
    newPos.z = targetCameraPos.z; // 補完しない(ステージが終わるまでzが加算されるから)

    // カメラ座標の設定と行列計算の更新
    camera_->SetTranslate(newPos);
    camera_->Update();
}
