#include "stageObject.h"

void stageObject::Initialize()
{
}

void stageObject::Update()
{
}

void stageObject::Draw()
{
}

AllAABB stageObject::GetAllAABB() const
{
    return AllAABB();
}

void stageObject::OnCollision(Collider* other)
{
    // 当たったもの次第で分岐
    if (other->GetCollisionGroup() == CollisionGroup::kEnemyBullet || other->GetCollisionGroup() == CollisionGroup::kEnenmy) {

        // 無敵時間のフラグ実行
    }
}
