#pragma once
#include <vector>

#include "Audio.h"
#include "Sound.h"
#include "WinApp.h"

class DirectXCommon;
class SrvManager;
class Input;
class ImGuiManager;
class SpriteCommon;
class Object3dCommon;
class ModelCommon;
class Camera;
class Sprite;
class Object3d;
class Model;
class ParticleEmitter;

class Game {
public:
    // 初期化
    void Initialize();

    // 更新
    void Update();

    // 描画
    void Draw();

    // 終了
    void Finalize();

    bool IsEndRequst() { return winApp->ProcessMessage(); }

private:
    // winApp
    WinApp* winApp = nullptr;
    // dxCommon
    DirectXCommon* dxCommon = nullptr;
    // srvマネージャー
    SrvManager* srvManager = nullptr;
    // input
    Input* input = nullptr;
    // Imguiマネージャー
    ImGuiManager* imguiManager = nullptr;
    // sprite
    SpriteCommon* spriteCommon = nullptr;
    // object3d
    Object3dCommon* object3dCommon = nullptr;
    // ModelCommon
    ModelCommon* modelCommon = nullptr;
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
