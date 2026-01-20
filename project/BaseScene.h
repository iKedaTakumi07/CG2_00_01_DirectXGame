#pragma once
class BaseScene {
public:
    virtual ~BaseScene() = default;

    // 初期化
    virtual void Initialize();

    // 終了
    virtual void Finalize();

    // 毎フレーム更新
    virtual void Update();

    // 描画
    virtual void Draw();
};
