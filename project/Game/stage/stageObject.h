#pragma once
#include "../../Engine/base/Math.h"
#include "../OnCollison/Collider.h"
#include <memory>

class Model;
class Object3d;
class Camera;

class stageObject : public Collider {
public:
    void Initialize();

    void Update();

    void Draw();

public:
    AllAABB GetAllAABB() const override;
    CollisionGroup GetCollisionGroup() const override { return CollisionGroup::kStageObject; }
    void OnCollision(Collider* other) override;

private:
private:
    Camera* camera_ = nullptr; // カメラポインタ

    std::unique_ptr<Model> ObjectModel;
    std::unique_ptr<Object3d> Object3d;
};
