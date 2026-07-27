#pragma once
#include "../../../Engine/base/Math.h"
class Camera;

class baseEnemyBullet {
public:
    virtual void Initialize(Camera* camera, Vector3 pos, const Vector3& rotation);

    virtual void Update(float deltaTime);

    virtual void Draw();

    // Get関数
    virtual bool GetIsDead() const = 0;
};
