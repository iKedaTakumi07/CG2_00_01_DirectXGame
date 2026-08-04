#include "CameraManager.h"
#include "../base/PostProcess.h"
#include "CPUParticle/CPUParticleManager.h"
#include "GPUParticleManager.h"
#include "Object3dCommon.h"
#include <cassert>

std::unique_ptr<CameraManager> CameraManager::instance_ = nullptr;

CameraManager* CameraManager::GetInstance()
{
    if (!instance_) {
        // privateコンストラクタのため new を使って unique_ptr を生成
        instance_ = std::unique_ptr<CameraManager>(new CameraManager());
    }
    return instance_.get();
}

Camera* CameraManager::CreateCamera(const std::string& name)
{
    auto camera = std::make_unique<Camera>();
    Camera* ptr = camera.get();
    cameras_[name] = std::move(camera);

    // 最初に作られたカメラを自動的にアクティブにする
    if (!activeCamera_) {
        SetActiveCamera(name);
    }

    return ptr;
}

void CameraManager::AddCamera(const std::string& name, std::unique_ptr<Camera> camera)
{
    assert(camera != nullptr);
    Camera* ptr = camera.get();
    cameras_[name] = std::move(camera);

    if (!activeCamera_) {
        SetActiveCamera(name);
    }
}

Camera* CameraManager::GetCamera(const std::string& name) const
{
    auto it = cameras_.find(name);
    if (it != cameras_.end()) {
        return it->second.get();
    }

    // ない場合エラーにする?適当にセットするかは、審議。
    return nullptr;
}

void CameraManager::SetActiveCamera(const std::string& name)
{
    Camera* target = GetCamera(name);
    if (target) {
        activeCamera_ = target;
        activeCameraName_ = name;

        // カメラ切り替え
        Object3dCommon::GetInstance()->SetDefaultCamera(activeCamera_);

        PostProcess::GetInstance()->SetDefaultCamera(activeCamera_);

        GPUParticleManager::getInstance()->SetDefaultCamera(activeCamera_);

        CPUParticleManager::getInstance()->SetDefaultCamera(activeCamera_);
    }
}

void CameraManager::Update()
{
    if (activeCamera_) {
        activeCamera_->Update();
    }
}

void CameraManager::Clear()
{
    // 安全設計にするならデフォルトカメラをこのクラス内に作る。
    cameras_.clear();
    activeCamera_ = nullptr;
    activeCameraName_ = "";
}
