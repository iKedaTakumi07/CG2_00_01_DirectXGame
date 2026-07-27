#include "EnemyManager.h"
#include "Normal/TargetEnemy.h"
#include "base/baseEnemy.h"

#include "../Player/Player.h"

void EnemyManager::Initialize(Player* player, Camera* camera)
{
    player_ = player;
    camera_ = camera;

    std::unique_ptr<baseEnemy> newEnemy = nullptr;
    newEnemy = std::make_unique<TargetEnemy>();
    Vector3 pos = { 0.0f, 0.0f, 50.0f };
    newEnemy->Initialize(camera_, pos);
    newEnemy->SetTargetPlayer(player_);

    enemies_.push_back(std::move(newEnemy));
}

void EnemyManager::Update()
{
    for (auto& enemy : enemies_) {
        enemy->Update();
    }
}

void EnemyManager::Draw()
{
    for (auto& enemy : enemies_) {
        enemy->Draw();
    }
}
