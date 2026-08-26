#include "stageObjectManager.h"
#include "../../Engine/3d/CameraManager.h"
#include "../../Engine/3d/ModelManager.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../../Engine/base/TextureManager.h"
#include "../Player/Player.h"

#include "../../resources/nlohmann/json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void stageObjectManager::Initialize(const std::string& filePath, Player* player)
{
    TextureManager::getInstance()->LoadTexture("resources/stage/uvChecker.png");
    ModelManager::GetInstance()->LoadModel("stage/stageGraunod.obj");

    PopObjFilePath_ = filePath;
    player_ = player;

    grauond3d = std::make_unique<Object3d>();
    grauond3d->Initialize();

    grauondModel = std::make_unique<Model>();
    grauondModel->Initialize("resources/stage", "stageGraunod.obj");
    grauond3d->SetModel(grauondModel.get());
    grauond3d->SetScale(grauondtransform_.scale);

    LoadJsonPopData(PopObjFilePath_);
}

void stageObjectManager::Update()
{
    if (player_) {
        Vector3 pos = player_->GetTranslate();
        while (currentSpawnIndex_ < popDatas_.size() && pos.z >= popDatas_[currentSpawnIndex_].spawn_Z) {
            PopEnemyCheck(popDatas_[currentSpawnIndex_]);
            currentSpawnIndex_++;
        }
    }

    for (auto& obj : stageObjects_) {
        obj->Update();
    }

    grauond3d->SetTranslate(grauondtransform_.translate);
    grauond3d->SetRotate(grauondtransform_.rotate);

    grauond3d->Update();
}

void stageObjectManager::Draw()
{
    grauond3d->Draw();

    for (auto& obj : stageObjects_) {
        obj->Draw();
    }
}

void stageObjectManager::LoadJsonPopData(const std::string& filePath)
{
    std::ifstream file(PopObjFilePath_);
    if (!file.is_open())
        return;

    json data; // ← 注意: ここの変数名も data
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return;
    }

    popDatas_.clear();
    if (data.contains("Object") && data["Object"].is_array()) {
        for (const auto& objData : data["Object"]) {
            stageObjectPopData popdata;

            // 絶対に代入するもの
            popdata.ObjectPatan = objData.value("ObjectPatan", "stageCube1");
            popdata.spawn_Z = objData.value("spawn_z", 0.0f);

            if (objData.contains("pos")) {
                const auto& posData = objData["pos"];
                popdata.popPosition.x = posData.value("x", 0.0f);
                popdata.popPosition.y = posData.value("y", 0.0f);
                popdata.popPosition.z = posData.value("z", 40.0f);
            } else {
                popdata.popPosition = { 0.0f, 0.0f, 40.0f };
            }

            if (objData.contains("Scale")) {
                const auto& scaleData = objData["Scale"];
                popdata.sizeScale.x = scaleData.value("x", 2.0f);
                popdata.sizeScale.y = scaleData.value("y", 2.0f);
                popdata.sizeScale.z = scaleData.value("z", 2.0f);
            } else {
                popdata.sizeScale = { 1.0f, 1.0f, 1.0f };
            }

            popDatas_.push_back(popdata);
        }
    }

    // sort
    std::sort(popDatas_.begin(), popDatas_.end(), [](const stageObjectPopData& a, const stageObjectPopData& b) {
        return a.spawn_Z < b.spawn_Z;
    });
}

void stageObjectManager::PopEnemyCheck(const stageObjectPopData& data)
{
    auto newObj = std::make_unique<stageObject>();
    newObj->Initialize(data.ObjectPatan, data.popPosition, data.sizeScale);
    stageObjects_.push_back(std::move(newObj));
}
