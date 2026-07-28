#pragma once
#include "Collider.h"
#include <list>

class CollisionManager {
public:
    // 判定対象
    void AddCollider(Collider* collider);

    void Clear();

    void CheckAllCollisions();

private:
    bool CheckAABB(const AABB& a, const AABB& b) const;

    std::list<Collider*> colliders_;
};
