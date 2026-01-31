#pragma once
#include "BaseScene.h"
#include <memory>
#include <vector>

class Model;
class Object3d;
#include "Sprite.h"
class ParticleEmitter;
#include "Sound.h"

class GamePlayScene : public BaseScene {
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
    std::unique_ptr<Model> model;
    std::unique_ptr<Model> model2;
    std::unique_ptr<Object3d> object3d;
    std::unique_ptr<Object3d> object3d2;

    // スプライト
    std::list<std::unique_ptr<Sprite>> sprites;

    // パーティクル
    ParticleEmitter* particleEmitter;

    // 音声データ
    Sound fanfare;
    Sound clearSe;
};
