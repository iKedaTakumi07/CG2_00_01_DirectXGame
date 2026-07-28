#include "GamePlayScene.h"
#include "../SceneManager.h"

#include "../../base/WinApp.h"

#include "../../2d/SpriteCommon.h"
#include "../../base/TextureManager.h"

#include "../../3d/Model.h"
#include "../../3d/ModelManager.h"
#include "../../3d/Object3d.h"
#include "../../3d/Object3dCommon.h"

#include "../../3d/CPUParticle/CPUParticleManager.h"
#include "../../3d/CPUParticle/ParticleEmitter.h"

#include "../../3d/Skybox/SkyBoxCommon.h"
#include "../../3d/Skybox/Skybox.h"

#include "../../io/Input.h"

#include "../../../Game/Enemy/EnemyManager.h"
#include "../../../Game/Enemy/base/baseEnemy.h"
#include "../../../Game/OnCollison/CollisionManager.h"
#include "../../../Game/Player/Player.h"

#include "math.h"

void GamePlayScene::Finalize()
{
    fanfare.Unload();
    clearSe.Unload();
}

GamePlayScene::GamePlayScene()
{
}

GamePlayScene::~GamePlayScene() = default;

void GamePlayScene::Initialize()
{

    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");
    TextureManager::getInstance()->LoadTexture("resources/monsterBall.png");
    TextureManager::getInstance()->LoadTexture("resources/rostock_laage_airport_4k.dds");

    skydox = std::make_unique<Skybox>();
    skydox->Initialize("resources/rostock_laage_airport_4k.dds");

    ModelManager::GetInstance()->LoadModel("Plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");

    fanfare.SoundLoadFile("resources/fanfare.wav");
    clearSe.SoundLoadFile("resources/stage.mp3");

    player_ = std::make_unique<Player>();
    player_->Initialize(BaseScene::GetCamera());

    enemyManager_ = std::make_unique<EnemyManager>();
    enemyManager_->Initialize(player_.get(), BaseScene::GetCamera());

    collisionManager_ = std::make_unique<CollisionManager>();

    // 音がうるさいので停止中
    // Audio::GetInstance()->Play(fanfare);
    // Audio::GetInstance()->Play(clearSe);
}

void GamePlayScene::Update()
{
    auto* input = Input::getInstance();
    Camera* camera = GetCamera();

    if (input->TriggerKey(DIK_F1)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }

    player_->Update();
    enemyManager_->Update();

    collisionManager_->Clear();
    collisionManager_->AddCollider(player_.get());
    for (auto& bullet : player_->GetBullets()) {
        collisionManager_->AddCollider(bullet.get());
    }
    for (auto& enemy : enemyManager_->GetEnemyes()) {
        collisionManager_->AddCollider(enemy.get());
        for (auto& enemyBullet : enemy->GetBullets()) {
            collisionManager_->AddCollider(enemyBullet.get());
        }
    }

    collisionManager_->CheckAllCollisions();
}

void GamePlayScene::Draw()
{

    Object3dCommon::GetInstance()->PrepareObjectDraw();

    //
    // モデルデータ
    //
    player_->Draw();
    enemyManager_->Draw();

    SkyBoxCommon::GetInstance()->PrepareObjectDraw();
    // skydox->Draw();

    //
    // 2d/スプライト
    //
    SpriteCommon::GetInstance()->PrepareSpriteDraw();

    CPUParticleManager::getInstance()->Draw();
}