#pragma once
#include "../../Engine/3d/Camera.h"
#include "../../Engine/3d/Model.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"

class stageObject {
public:
    void Initialize();

    void Update();

    void Draw();

private:
    Transform grauondtransform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // 座標

    std::unique_ptr<Model> grauondModel;
    std::unique_ptr<Object3d> grauond3d;
};
