#include "ModelManager.h"
#include "DirectXCommon.h"
#include "Model.h"
#include "ModelCommon.h"
#include <cassert>

ModelManager* ModelManager::instance = nullptr;
bool ModelManager::finalized = false;

void ModelManager::LoadModel(const std::string& filePath)
{
    if (models.contains(filePath)) {
        return;
    }

    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Initialize(/*modelCommon,*/ "resources", filePath);

    models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
    if (models.contains(filePath)) {
        return models.at(filePath).get();
    }

    // 一致なし
    return nullptr;
}

ModelManager* ModelManager::GetInstance()
{
    // Finalize後に呼ばれた場合はエラー
    assert(!finalized && "ModelManager was already finalized!");

    if (instance == nullptr) {
        instance = new ModelManager();
    }
    return instance;
}

void ModelManager::Finalize()
{
    models.clear(); // u
}

void ModelManager::Initialize(DirectXCommon* dxCommon)
{
    // modelCommon = new ModelCommon;
    ModelCommon::GetInstance()->Initialize(dxCommon);
}