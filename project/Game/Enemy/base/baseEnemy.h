#pragma once

#include "../../../Engine/base/Math.h"
class Camera;

class baseEnemy {
public:
    virtual void Initialize(Camera* camera, Vector3 pos);

    virtual void Update();

    virtual void Draw();
};
