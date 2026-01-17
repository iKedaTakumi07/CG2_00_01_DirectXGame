#pragma once
#include <vector>

#include "Audio.h"
#include "Framework.h"
#include "Sound.h"



class Camera;
class Sprite;
class Object3d;
class Model;
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

private:
    // ゲーム終了クラス
    bool endRequst_ = false;

    // カメラ
    Camera* camera = nullptr;

    // Audio
    Audio audio;

    ///
    /// その他
    ///

    // 3dモデル
    Model* model;
    Model* model2;
    Object3d* object3d;
    Object3d* object3d2;

    // パーティクル
    ParticleEmitter* particleEmitter;

    // スプライト
    std::vector<Sprite*> sprites;
    bool isSprite = true;

    // 音声データ
    Sound fanfare;
    Sound clearSe;
};
