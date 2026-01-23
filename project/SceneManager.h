#pragma once

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
    void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

private:
    SceneManager() = default;
    ~SceneManager() = default;

private:
    BaseScene* nextScene_ = nullptr;
    BaseScene* scene_ = nullptr;
};
