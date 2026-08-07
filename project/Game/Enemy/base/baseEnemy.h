#pragma once

#include "../../../Engine/base/Math.h"
#include "baseEnemyBullet.h"
#include <memory>
class Camera;
class Player;

#include "../../OnCollison/Collider.h"

class baseEnemy : public Collider {
public:
    virtual void Initialize(Vector3 pos);

    virtual void Update();

    virtual void Draw();

    /* Set関数 */
    virtual void SetTargetPlayer(Player* target) { }; // 対象に向かわせる

    /* Get関数 */
    const std::vector<std::unique_ptr<baseEnemyBullet>>& GetBullets() const { return enemyBullet_; }

protected:
    // 弾
    std::vector<std::unique_ptr<baseEnemyBullet>> enemyBullet_;
};
