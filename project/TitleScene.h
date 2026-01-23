#pragma once
#include "BaseScene.h"

class Model;
class Object3d;
class Input;

class TitleScene : public BaseScene {
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
    // 3dモデル
    Model* model;
    Model* model2;
    Object3d* object3d;
    Object3d* object3d2;
};
