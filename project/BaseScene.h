#pragma once


class SceneManager;
class Input;

class BaseScene {
public:
    virtual ~BaseScene() = default;

    // 初期化
    virtual void Initialize();

    // 終了
    virtual void Finalize();

    // 毎フレーム更新
    virtual void Update();

    // 描画
    virtual void Draw();

    virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

    // set
    void SetInput(Input* input) { input_ = input; }
    // get
    virtual SceneManager* GetSceneManager() { return sceneManager_; }
    Input* GetInput() { return input_; }

private:
    // シーンマネージャー(貸出)
    SceneManager* sceneManager_ = nullptr;
    Input* input_ = nullptr;
};
