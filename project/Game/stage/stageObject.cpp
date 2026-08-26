#include "stageObject.h"

#include "../../Engine/3d/CameraManager.h"
#include "../../Engine/3d/ModelManager.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../../Engine/base/TextureManager.h"

void stageObject::Initialize(const std::string& patan, const Vector3& pos, const Vector3& scale)
{
    TextureManager::getInstance()->LoadTexture("resources/stage/uvChecker.png");
    ModelManager::GetInstance()->LoadModel("stage/stageCube1.obj");
    ModelManager::GetInstance()->LoadModel("stage/stageObjectCube.obj");

    camera_ = CameraManager::GetInstance()->GetActiveCamera();
    objPatan_ = patan;

    transform_.translate = pos;
    transform_.scale = scale;
    transform_.rotate = { 0.0f, 0.0f, 0.0f };

    Object3d_ = std::make_unique<Object3d>();
    Object3d_->Initialize();

    ObjectModel = std::make_unique<Model>();
    ObjectModel->Initialize("resources/stage", objPatan_ + ".obj"); // 指定したパターンに
    Object3d_->SetModel(ObjectModel.get());

    Object3d_->SetTranslate(transform_.translate);
    Object3d_->SetScale(transform_.scale);
    Object3d_->SetRotate(transform_.rotate);
}

void stageObject::Update()
{
    camera_ = CameraManager::GetInstance()->GetActiveCamera();

    Object3d_->SetTranslate(transform_.translate);
    Object3d_->SetScale(transform_.scale);
    Object3d_->SetRotate(transform_.rotate);
    Object3d_->Update();
}

void stageObject::Draw()
{
    Object3d_->Draw();
}

AllAABB stageObject::GetAllAABB() const
{
    AABB aabb;
    AllAABB compound;
    float sizeX = baseSize_ * transform_.scale.x;
    float sizeY = baseSize_ * transform_.scale.y;
    float sizeZ = baseSize_ * transform_.scale.z;

    float halfSizeX = sizeX * 0.5f;
    float halfSizeY = sizeY * 0.5f;

    // 全体の大きさ(早期リターン用)
    aabb.min = { transform_.translate.x - sizeX, transform_.translate.y - sizeY, transform_.translate.z - sizeZ };
    aabb.max = { transform_.translate.x + sizeX, transform_.translate.y + sizeY, transform_.translate.z + sizeZ };

    compound.wholeBox = aabb;
    if (objPatan_ == "stageObjectCube") {
        compound.dividBoxes.push_back(aabb);
    } else if (objPatan_ == "stageCube1") {
        // 3分割した当たり判定にする

        AABB leftWall; // 左壁
        leftWall.min = { transform_.translate.x - sizeX, transform_.translate.y - sizeY, transform_.translate.z - sizeZ };
        leftWall.max = { transform_.translate.x - halfSizeX, transform_.translate.y + halfSizeY, transform_.translate.z + sizeZ };
        compound.dividBoxes.push_back(leftWall);

        AABB rightWall; // 右壁
        rightWall.min = { transform_.translate.x + halfSizeX, transform_.translate.y - sizeY, transform_.translate.z - sizeZ };
        rightWall.max = { transform_.translate.x + sizeX, transform_.translate.y + halfSizeY, transform_.translate.z + sizeZ };
        compound.dividBoxes.push_back(rightWall);

        AABB roof;
        roof.min = { transform_.translate.x - sizeX, transform_.translate.y + halfSizeY, transform_.translate.z - sizeZ };
        roof.max = { transform_.translate.x + sizeX, transform_.translate.y + sizeY, transform_.translate.z + sizeZ };
        compound.dividBoxes.push_back(roof);

    } else {
        // エラー対策
        compound.dividBoxes.push_back(aabb);
    }

    return compound;
}

void stageObject::OnCollision(Collider* other)
{
    // 当たったもの次第で分岐
    if (other->GetCollisionGroup() == CollisionGroup::kEnemyBullet || other->GetCollisionGroup() == CollisionGroup::kEnenmy) {

        // 無敵時間のフラグ実行
    }
}
