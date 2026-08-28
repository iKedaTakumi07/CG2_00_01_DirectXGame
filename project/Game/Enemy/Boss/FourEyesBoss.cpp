#include "FourEyesBoss.h"
#include <numbers>

#include "../../../Engine/3d/CameraManager.h"
#include "../../../Engine/3d/Model.h"
#include "../../../Engine/3d/ModelManager.h"

#include "../../../Engine/base/TextureManager.h"
#include "../../Player/Player.h"

#include "../../../Engine/3d/Object3d.h"
#include "../../../Engine/scene/SceneManager.h"
#include "../Bullet/EnemyHomingBullet.h"
#include "../Bullet/TargetBullet.h"
#include <memory>

void FourEyesBoss::Initialize(Vector3 pos)
{
    // スポーン位置を中心座標としてセット
    centerPos_ = pos;

    TextureManager::getInstance()->LoadTexture("resources/baseEnemy/uvChecker.png");
    ModelManager::GetInstance()->LoadModel("baseEnemy/bossEnemy.obj");

    object3d = std::make_unique<Object3d>();
    object3d->Initialize();

    isAvile_ = true;
    isDead_ = false;

    model = std::make_unique<Model>();
    model->Initialize("resources/baseEnemy", "bossEnemy.obj");
    // model->SetEvnTexturefilePath(skydox->GetTextureFilePath()); // 反射が必要なら
    object3d->SetModel(model.get());

    transform_.scale = { 1.0f, 1.0f, 1.0f };
    transform_.rotate = { 0.0f, 0.0f, 0.0f };
    transform_.translate = centerPos_;
}

void FourEyesBoss::Update()
{
    float deltaTime = SceneManager::GetInstance()->GetDeltaTime();
    camera_ = CameraManager::GetInstance()->GetActiveCamera();

    // 発射処理
    MoveUpdate();
    FireFourWayBullets();

    object3d->Update();
}

void FourEyesBoss::Draw()
{
    object3d->Draw();

    for (const auto& bullet : enemyBullet_) {
        if (bullet) {
            bullet->Draw();
        }
    }
}

void FourEyesBoss::SpriteDraw()
{
}

AllAABB FourEyesBoss::GetAllAABB() const
{
    AABB aabb;
    AllAABB compound;

    // 早期リターン回避用に上下左右の全体を入れる
    aabb.min = { transform_.translate.x - 7.5f, transform_.translate.y - 7.5f, transform_.translate.z - 3.0f };
    aabb.max = { transform_.translate.x + 7.5f, transform_.translate.y + 7.5f, transform_.translate.z + 1.5f };
    compound.wholeBox = aabb;

    AABB leftEyes; // 左
    leftEyes.min = { transform_.translate.x + 4.5f, transform_.translate.y - 1.5f, transform_.translate.z - 3.0f };
    leftEyes.max = { transform_.translate.x + 7.5f, transform_.translate.y + 1.5f, transform_.translate.z + 1.5f };
    compound.dividBoxes.push_back(leftEyes);

    AABB rightEyes; // 右
    rightEyes.min = { transform_.translate.x - 7.5f, transform_.translate.y - 1.5f, transform_.translate.z - 3.0f };
    rightEyes.max = { transform_.translate.x - 4.5f, transform_.translate.y + 1.5f, transform_.translate.z + 1.5f };
    compound.dividBoxes.push_back(rightEyes);

    AABB TopEyes; // 上
    TopEyes.min = { transform_.translate.x - 1.5f, transform_.translate.y + 4.5f, transform_.translate.z - 3.0f };
    TopEyes.max = { transform_.translate.x + 1.5f, transform_.translate.y + 7.5f, transform_.translate.z + 1.5f };
    compound.dividBoxes.push_back(TopEyes);

    AABB BottomEyes; // 下
    BottomEyes.min = { transform_.translate.x - 1.5f, transform_.translate.y - 7.5f, transform_.translate.z - 3.0f };
    BottomEyes.max = { transform_.translate.x + 1.5f, transform_.translate.y - 4.5f, transform_.translate.z + 1.5f };
    compound.dividBoxes.push_back(BottomEyes);

    return compound;
}

std::vector<Vector3> FourEyesBoss::GetTargetPositions()
{
    std::vector<Vector3> positions;

    // 各目の位置を座標に登録
    for (const auto& offset : muzzleOffsets_) {
        positions.push_back({ transform_.translate.x + offset.x,
            transform_.translate.y + offset.y,
            transform_.translate.z + offset.z });
    }
    return positions;
}

void FourEyesBoss::OnCollision(Collider* other)
{
    // 当たったもの次第で分岐
    if (other->GetCollisionGroup() == CollisionGroup::kPlayerBullet) {
        // ダメージ処理
        currentHp_ -= other->GetDamage();

        if (currentHp_ <= 0) {
            isAvile_ = false; // 死亡演出作ったならそっちに移行
            isDead_ = true; // 死亡演出トリガー用
        }
    } else if (other->GetCollisionGroup() == CollisionGroup::kPlayer) {
        // お互いダメージ処理
        currentHp_ -= 1;

        if (currentHp_ <= 0) {
            isAvile_ = false; // 死亡演出作ったならそっちに移行
            isDead_ = true; // 死亡演出トリガー用
        }
    }
}

void FourEyesBoss::StartAppearance()
{
}

void FourEyesBoss::UpdateAppearance(float deltaTime)
{
}

void FourEyesBoss::FireFourWayBullets()
{
    float currentDeltaTime = SceneManager::GetInstance()->GetDeltaTime();
    interval -= currentDeltaTime;

    if (interval <= 0.0f) {
        // 弾の生成
        for (int i = 0; i < 4; i++) {
            std::unique_ptr<EnemyHomingBullet> newBulletEnemy = std::make_unique<EnemyHomingBullet>();
            Vector3 pos = {
                transform_.translate.x + muzzleOffsets_[i].x,
                transform_.translate.y + muzzleOffsets_[i].y,
                transform_.translate.z + muzzleOffsets_[i].z
            };
            newBulletEnemy->Initialize(pos, transform_.rotate);
            newBulletEnemy->SetTargetPosition(player_->GetTranslate());

            enemyBullet_.push_back(std::move(newBulletEnemy));
        }
        interval = maxInterval;
    }

    // 更新処理
    for (auto& bullet : enemyBullet_) {
        bullet->SetPlayerPos(player_->GetTranslate());
        bullet->Update(currentDeltaTime);
    }

    // 弾の削除
    std::erase_if(enemyBullet_, [](const std::unique_ptr<baseEnemyBullet>& bullet) {
        return bullet->GetIsDead(); // GetIsDead が true なら削除
    });
}

void FourEyesBoss::MoveUpdate()
{
    Vector3 pos = CameraManager::GetInstance()->GetActiveCamera()->GetTranslate();
    pos.z = pos.z + offsetPosZ;
    transform_.translate.z = pos.z;

    object3d->SetTranslate(transform_.translate);
    object3d->SetRotate(transform_.rotate);
}
