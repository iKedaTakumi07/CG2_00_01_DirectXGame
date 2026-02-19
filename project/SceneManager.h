#pragma once
#include <memory>

class BaseScene;

class SceneManager {
public:
    // コンストラクタに渡すための鍵
    class ConstructorKey {
    private:
        ConstructorKey() = default;
        friend class SceneManager;
    };

    // passkeyを受け取るコンストラクタ
    explicit SceneManager(ConstructorKey) { }

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
     friend struct std::default_delete<SceneManager>;
    ~SceneManager() = default;

private:
    std::unique_ptr<BaseScene> nextScene_ = nullptr;
    std::unique_ptr<BaseScene> scene_ = nullptr;
};
