#pragma once
#include "../base/BaseScene.h"

class resultScene : public BaseScene {
public:
    resultScene();
    ~resultScene();

public:
    // 初期化
    void Initialize() override;

    // 終了
    void Finalize() override;

    // 毎フレーム更新
    void Update() override;

    // 描画
    void Draw() override;
};
