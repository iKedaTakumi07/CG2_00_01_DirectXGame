#pragma once

class WinApp;
class DirectXCommon;
class SrvManager;
class Input;
class ImGuiManager;
class SpriteCommon;
class Object3dCommon;
class ModelCommon;

class Framework {
public:
    virtual ~Framework() = default;

    // 実行
    void Run();

    // 初期化
    virtual void Initialize();

    // 更新
    virtual void Update();

    // 描画
    virtual void Draw() = 0;

    // 終了
    virtual void Finalize();

    virtual bool IsEndRequst() { return endRequst_; }

private:
    // ゲーム終了クラス
    bool endRequst_ = false;

    // winApp
    WinApp* winApp = nullptr;
    // dxCommon
    DirectXCommon* dxCommon = nullptr;
    // srvマネージャー
    SrvManager* srvManager = nullptr;
    // input
    Input* input = nullptr;
    // Imguiマネージャー
    ImGuiManager* imguiManager = nullptr;
    // sprite
    SpriteCommon* spriteCommon = nullptr;
    // object3d
    Object3dCommon* object3dCommon = nullptr;
    // ModelCommon
    ModelCommon* modelCommon = nullptr;
};
