#pragma once
#include "BaseScene.h"
#include <memory>

#include "../2d/Sprite.h"
#include "../3d/ParticleEmitter.h"
#include "../audio/Sound.h"
class Model;
class Object3d;
class Skybox;

class TitleScene : public BaseScene {
public:
    TitleScene();
    ~TitleScene();

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
    std::unique_ptr<Object3d> object3d;

    std::unique_ptr<Model> model_2;
    std::unique_ptr<Object3d> object3d_2;

    std::unique_ptr<Skybox> skydox;

    std::unique_ptr<ParticleEmitter> particleEmitter;
    std::unique_ptr<ParticleEmitter> particleEmitter2;
    std::unique_ptr<ParticleEmitter> particleEmitter3;
};
