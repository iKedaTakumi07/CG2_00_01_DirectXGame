#include "EnemyManager.h"
#include "Normal/TargetEnemy.h"
#include "base/baseEnemy.h"

void EnemyManager::Initialize(Camera* camera)
{
    camera_ = camera;

    std::unique_ptr<baseEnemy> newEnemy = nullptr;
    newEnemy = std::make_unique<TargetEnemy>();
    Vector3 pos = { 0.0f, 0.0f, 50.0f };
    newEnemy->Initialize(camera_, pos);

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
