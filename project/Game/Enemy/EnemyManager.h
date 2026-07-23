#pragma once
#include <list>
#include <memory>

class baseEnemy;
class TargetEnemy;
class Camera;

class EnemyManager {
public:
    void Initialize(Camera* camera);

    void Update();

    void Draw();

private:
    Camera* camera_ = nullptr; // カメラポインタ

    std::list<std::shared_ptr<baseEnemy>> enemies_; // 生きている敵のリスト
};
