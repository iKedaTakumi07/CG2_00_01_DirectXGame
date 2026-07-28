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

public:
    // Get関数
    const std::list<std::unique_ptr<baseEnemy>>& GetEnemyes() const { return enemies_; } // 弾の入手


private:
    Camera* camera_ = nullptr; // カメラポインタ
    Player* player_ = nullptr; // ターゲット用のプレイヤーポインタ

    std::list<std::unique_ptr<baseEnemy>> enemies_; // 生きている敵のリスト
};
