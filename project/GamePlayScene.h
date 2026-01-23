#pragma once
#include "BaseScene.h"
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
    Model* model;
    Model* model2;
    Object3d* object3d;
    Object3d* object3d2;

    // スプライト
    std::list<Sprite*> sprites;

    // パーティクル
    ParticleEmitter* particleEmitter;

    // 音声データ
    Sound fanfare;
    Sound clearSe;
};
