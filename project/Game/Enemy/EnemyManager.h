#pragma once

#include <list>
#include <memory>

class baseEnemy;
class TargetEnemy;
class Camera;
class Player;

class EnemyManager {
public:
    void Initialize(Player* player, Camera* camera);

    void Update();

    void Draw();

private:
    Camera* camera_ = nullptr; // カメラポインタ
    Player* player_ = nullptr; // ターゲット用のプレイヤーポインタ

    std::list<std::shared_ptr<baseEnemy>> enemies_; // 生きている敵のリスト
};
