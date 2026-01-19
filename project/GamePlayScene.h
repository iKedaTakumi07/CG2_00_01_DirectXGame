#pragma once
#include <vector>

class Model;
class Object3d;
#include "Sprite.h"
class ParticleEmitter;
#include "Sound.h"

class GamePlayScene {
public:
    // 初期化
    void Initialize();

    // 終了
    void Finalize();

    // 毎フレーム更新
    void Update();

    // 描画
    void Draw();

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
