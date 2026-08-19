#include "stageObjectManager.h"
#include "../../Engine/3d/CameraManager.h"
#include "../../Engine/3d/ModelManager.h"
#include "../../Engine/3d/Object3d.h"
#include "../../Engine/base/Math.h"
#include "../../Engine/base/TextureManager.h"

#include "../../resources/nlohmann/json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void stageObjectManager::Initialize(const std::string& filePath)
{
    TextureManager::getInstance()->LoadTexture("resources/stage/uvChecker.png");
    ModelManager::GetInstance()->LoadModel("stage/stageGraunod.obj");

    PopEnemyFilePath_ = filePath;

    grauond3d = std::make_unique<Object3d>();
    grauond3d->Initialize();

    grauondModel = std::make_unique<Model>();
    grauondModel->Initialize("resources/stage", "stageGraunod.obj");
    grauond3d->SetModel(grauondModel.get());
    grauond3d->SetScale(grauondtransform_.scale);

    LoadJsonPopData(PopEnemyFilePath_);
}

void stageObjectManager::Update()
{
    grauond3d->SetTranslate(grauondtransform_.translate);
    grauond3d->SetRotate(grauondtransform_.rotate);

    grauond3d->Update();
}

void stageObjectManager::Draw()
{
    grauond3d->Draw();
}

void stageObjectManager::LoadJsonPopData(const std::string& filePath)
{
    std::ifstream file(PopEnemyFilePath_);
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
}

void stageObjectManager::PopEnemyCheck(const stageObjectPopData& data)
{
}
