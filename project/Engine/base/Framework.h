#pragma once
#include <memory>

#include "../3d/Camera.h"
#include "../io/Input.h"
#include "../scene/BaseScene.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "SrvManager.h"
class WinApp;
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

    // get
    BaseScene* GetBaseScene() { return baseScene.get(); }
    DirectXCommon* GetDirectXCommon() { return dxCommon.get(); }
    SrvManager* GetSrvManager() { return srvManager.get(); }
    Input* GetInput() { return input.get(); }
    ImGuiManager* GetImGuiManager() { return imguiManager.get(); }
    Camera* GetCamera() { return camera.get(); }

private:
    // ゲーム終了クラス
    bool endRequst_ = false;

    // dxCommon
    std::unique_ptr<DirectXCommon> dxCommon = nullptr;
    // srvマネージャー
    std::unique_ptr<SrvManager> srvManager = nullptr;
    // input
    std::unique_ptr<Input> input = nullptr;
    // IMGUIマネージャー
    std::unique_ptr<ImGuiManager> imguiManager = nullptr;
    // カメラ
    std::unique_ptr<Camera> camera = nullptr;
    // ベースシーン
    std::unique_ptr<BaseScene> baseScene = nullptr;
};
