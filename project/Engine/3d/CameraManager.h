#pragma once
#include "Camera.h"
#include <memory>
#include <string>
#include <unordered_map>

class CameraManager {
    friend struct std::default_delete<CameraManager>;

public:
    static CameraManager* GetInstance();

    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

public:
    /// <summary>
    /// カメラ新規作成
    /// </summary>
    /// <param name="name">名前("重複不可")</param>
    /// <returns></returns>
    Camera* CreateCamera(const std::string& name);

    /// <summary>
    /// 既存のカメラを登録(多分使う気がしない)
    /// </summary>
    void AddCamera(const std::string& name, std::unique_ptr<Camera> camera);

    /// <summary>
    /// 名前からカメラを取得
    /// </summary>
    Camera* GetCamera(const std::string& name) const;

    /// <summary>
    /// アクティブなカメラを切り替える（Object3dCommonなどのを全て変更）
    /// </summary>
    /// <param name="name">切り替えるカメラの名前</param>
    void SetActiveCamera(const std::string& name);

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 登録されているカメラを全削除
    /// </summary>
    void Clear();

    // get
    Camera* GetActiveCamera() const { return activeCamera_; } // カメラのポインタ
    const std::string& GetActiveCameraName() const { return activeCameraName_; } // カメラの名前

private:
    CameraManager() = default;
    ~CameraManager() = default;

private:
    static std::unique_ptr<CameraManager> instance_;

    std::unordered_map<std::string, std::unique_ptr<Camera>> cameras_; // 登録したカメラたち
    Camera* activeCamera_ = nullptr; // 現在使用されているカメラ
    std::string activeCameraName_ = ""; // 現在使用されているカメラ(名前)
};
