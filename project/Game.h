#pragma once
#include <vector>

#include "Framework.h"
#include "SceneManager.h"
#include "Sound.h"

class Sprite;
class Object3d;
class Model;

class GamePlayScene;
// class BaseScene;

class ParticleEmitter;

class Game : public Framework {
public:
    // 初期化
    void Initialize() override;

    // 更新
    void Update() override;

    // 描画
    void Draw() override;

    // 終了
    void Finalize() override;

    // get
    Input* GetInput() { return input_; }

private:
    ///
    /// その他
    ///

    Framework* framework = nullptr;
    // BaseScene* baseScene = nullptr;
    SceneManager* sceneManager_ = nullptr;

    Input* input_ = nullptr;

    // GamePlayScene* scene_ = nullptr;
};
