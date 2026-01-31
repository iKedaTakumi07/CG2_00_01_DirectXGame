#include "SceneManager.h"
#include "BaseScene.h"

SceneManager* SceneManager::GetInstance()
{

    static std::unique_ptr<SceneManager> instance  (new SceneManager());

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
