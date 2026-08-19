#pragma once
#include "../../Engine/3d/Camera.h"
#include "../../Engine/3d/Model.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "stageObject.h"

#include <memory>
#include <vector>

struct stageObjectPopData {
    std::string enemyPopType; // 敵の種類
    float spawn_Z; // 出現するタイミング
    Vector3 popPosition; // 出現場所
    std::string useBullet; // 使う弾の種類
    int hp; // 体力

    // 動く系のみ
    Vector3 moveDirection; // 進行方向
    //

    // 追尾弾のみ
    float homingPower;
    //
};

class stageObjectManager {
public:
    void Initialize(const std::string& filePath);

    void Update();

    void Draw();

public:
    void LoadJsonPopData(const std::string& filePath);

private:
    void PopEnemyCheck(const stageObjectPopData& data);

private:
    std::vector<stageObjectPopData> popDatas_; // 読み込んだデータ
    std::vector<std::unique_ptr<stageObject>> stageObjects_; // 生きている敵のリスト

    size_t currentSpawnIndex_ = 0; // 次に出現させる敵のインデックス
    std::string PopEnemyFilePath_; // 現在読み込んでいるファイル

    Transform grauondtransform_ = { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }; // 座標

    std::unique_ptr<Model> grauondModel;
    std::unique_ptr<Object3d> grauond3d;
};
