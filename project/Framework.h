#pragma once
#include <memory>

class WinApp;
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "Input.h"
#include "ImGuiManager.h"
class SpriteCommon;
class Object3dCommon;
class ModelCommon;
#include "Camera.h"
#include "BaseScene.h"
// #include "Audio.h"

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
    virtual void Draw();

    // 終了
    virtual void Finalize();

    virtual bool IsEndRequst() { return endRequst_; }

    // get
    BaseScene* GetBaseScene() { return baseScene.get(); }
    DirectXCommon* GetDirectXCommon() { return dxCommon.get(); }
    SrvManager* GetSrvManager() { return srvManager.get(); }
    Input* GetInput() { return input.get(); }
    ImGuiManager* GetImGuiManager() { return imguiManager.get(); }

private:
    // ゲーム終了クラス
    bool endRequst_ = false;

    // winApp
    // WinApp* winApp = nullptr;
    // dxCommon
    std::unique_ptr<DirectXCommon> dxCommon = nullptr;
    // srvマネージャー
    std::unique_ptr<SrvManager> srvManager = nullptr;
    // input
    std::unique_ptr<Input> input = nullptr;
    // Imguiマネージャー
    std::unique_ptr<ImGuiManager> imguiManager = nullptr;
    // sprite
    // SpriteCommon* spriteCommon = nullptr;
    // object3d
    // Object3dCommon* object3dCommon = nullptr;
    // ModelCommon
    // ModelCommon* modelCommon = nullptr;
    // カメラ
    std::unique_ptr<Camera> camera = nullptr;

    std::unique_ptr<BaseScene> baseScene = nullptr;
    // Audio
    // Audio* audio;
};
