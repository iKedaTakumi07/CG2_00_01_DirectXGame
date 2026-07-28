#include "CollisionManager.h"

void CollisionManager::AddCollider(Collider* collider)
{
    colliders_.push_back(collider);
}

void CollisionManager::Clear()
{
    colliders_.clear();
}

void CollisionManager::CheckAllCollisions()
{
    // 総当たり
    auto itA = colliders_.begin();
    for (; itA != colliders_.end(); ++itA) {
        Collider* colA = *itA;

        auto itB = itA;
        ++itB; // 次の要素から比較
        for (; itB != colliders_.end(); ++itB) {
            Collider* colB = *itB;

            // 同じ陣営はスキップ
            if (colA->GetCollisionGroup() == colB->GetCollisionGroup())
                continue;
            if (colA->GetCollisionGroup() == CollisionGroup::kPlayer && colB->GetCollisionGroup() == CollisionGroup::kPlayerBullet)
                continue;
            if (colA->GetCollisionGroup() == CollisionGroup::kPlayerBullet && colB->GetCollisionGroup() == CollisionGroup::kPlayer)
                continue;

            AABB aabbA = colA->GetAABB();
            AABB aabbB = colB->GetAABB();

            if (CheckAABB(aabbA, aabbB)) {
                // 交差していれば、お互いのオーバーライドされたOnCollisionを呼び出す
                colA->OnCollision(colB);
                colB->OnCollision(colA);
            }
        }
    }
}

bool CollisionManager::CheckAABB(const AABB& a, const AABB& b) const
{
    // AABB
    if (a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y && a.max.y >= b.min.y && a.min.z <= b.max.z && a.max.z >= b.min.z) {
        return true;
    }
    return false;
}
