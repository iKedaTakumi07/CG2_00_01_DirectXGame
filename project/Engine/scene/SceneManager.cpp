#include "SceneManager.h"
#include "BaseScene.h"
#include <assert.h>

SceneManager* SceneManager::GetInstance()
{
    static std::unique_ptr<SceneManager> instance = std::make_unique<SceneManager>(ConstructorKey());

    return instance.get();
}

void SceneManager::Finalize()
{
    if (scene_) {
        scene_->Finalize();
        scene_.reset();
    }
}

void SceneManager::Update()
{
    if (nextScene_) {
        if (scene_) {
            scene_->Finalize();
        }

        scene_ = std::move(nextScene_);
        scene_->SetSceneManager(this);

        // シーンの初期化
        scene_->Initialize();
    }

    scene_->Update();
}

void SceneManager::Draw()
{
    scene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
    assert(sceneFactory_);
    assert(nextScene_ == nullptr);

    // 次シーンを生成。
    nextScene_ = sceneFactory_->CreateScene(sceneName);
}
