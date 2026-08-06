#include "EnemyManager.h"
#include "Normal/HomingEnemy.h"
#include "Normal/TargetEnemy.h"
#include "base/baseEnemy.h"

#include "../Player/Player.h"

void EnemyManager::Initialize(Player* player, Camera* camera)
{
    player_ = player;
    camera_ = camera;

    for (int i = 0; i < 2; i++) {
        std::unique_ptr<baseEnemy> newEnemy = nullptr;
        Vector3 pos = { 0.0f, 0.0f, 50.0f };
        if (i == 1) {
            newEnemy = std::make_unique<TargetEnemy>();
            pos = { 0.0f, 0.0f, 50.0f };
            newEnemy->Initialize(pos);
            newEnemy->SetTargetPlayer(player_);
        } else {
            newEnemy = std::make_unique<HomingEnemy>();
            pos = { 0.0f, 5.0f, 50.0f };
            newEnemy->Initialize(pos);
            newEnemy->SetTargetPlayer(player_);
        }
        enemies_.push_back(std::move(newEnemy));
    }
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
