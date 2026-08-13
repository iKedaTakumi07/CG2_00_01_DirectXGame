#pragma once
#include "../base/BaseScene.h"
#include <memory>

class Player;

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

private:
    std::unique_ptr<Player> player_;
};
