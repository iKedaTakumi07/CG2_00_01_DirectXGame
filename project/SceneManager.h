#pragma once
#include <memory>

class BaseScene;

class SceneManager {
public:
    // Singleton 取得
    static SceneManager* GetInstance();

    // 終了
    void Finalize();

    // 更新
    void Update();

    // 描画
    void Draw();

    // 次のシーン予約
    void SetNextScene(std::unique_ptr<BaseScene> nextScene) { nextScene_ = std::move(nextScene); }

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

private:
    SceneManager() = default;
    ~SceneManager() = default;

    friend struct std::default_delete<SceneManager>;

private:
    std::unique_ptr<BaseScene> nextScene_ = nullptr;
    std::unique_ptr<BaseScene> scene_ = nullptr;
};
