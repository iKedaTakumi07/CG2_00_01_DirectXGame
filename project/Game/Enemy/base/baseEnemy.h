#pragma once

#include "../../../Engine/base/Math.h"
#include "baseEnemyBullet.h"
#include <memory>
class Camera;
class Player;

class baseEnemy {
public:
    virtual void Initialize(Camera* camera, Vector3 pos);

    virtual void Update();

    virtual void Draw();

    /* Set関数 */
    virtual void SetTargetPlayer(Player* target) { };

protected:
    // 弾
    std::vector<std::unique_ptr<baseEnemyBullet>> enemyBullet_;
};
