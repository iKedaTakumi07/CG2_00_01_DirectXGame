#pragma once
#include "BaseScene.h"
#include <list>
#include <memory>
#include <vector>

#include "../2d/Sprite.h"
#include "../3d/CPUParticle/ParticleEmitter.h"
#include "../audio/Sound.h"

class Model;
class Object3d;
class Skybox;
class Player;
class EnemyManager;
class CollisionManager;

class GamePlayScene : public BaseScene {
public:
    GamePlayScene();
    ~GamePlayScene();

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
    std::unique_ptr<Skybox> skydox;

    // プレイヤー
    std::unique_ptr<Player> player_;
    // 敵
    std::unique_ptr<EnemyManager> enemyManager_;

    // 当たり半テオ
    std::unique_ptr<CollisionManager> collisionManager_;

    // 音声データ
    Sound fanfare;
    Sound clearSe;
};
