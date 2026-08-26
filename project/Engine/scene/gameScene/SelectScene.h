#pragma once
#include "../../2d/Sprite.h"
#include "../base/BaseScene.h"
#include <memory>

class SelectScene : public BaseScene {
public:
    SelectScene();
    ~SelectScene();

public:
    // 初期化
    void Initialize() override;

    // 終了
    void Finalize() override;

    // 毎フレーム更新
    void Update() override;

    // 描画
    void Draw() override;

private:
    int stageNumber;
    int MaxStageNumber = 2;
    int MinStageNumber = 1;

    bool selectStop = false;

    // UI(スプライト)
    std::unique_ptr<Sprite> SatgeUI1;
    std::unique_ptr<Sprite> SatgeUI2;
};
