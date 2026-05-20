#pragma once
#include "DirectXCommon.h"
#include <memory>
class Camera;

class PostProcess {
public:
    enum class Mode {
        kNormal, // 通常コピー
        kGrayscale, // グレースケール
        kSepiascale, // セピア調
        kVignette, // ヴィネッティング
        kBoxFillter, // ボックスフィルター(3x3のカーネル)
        kBoxFillter5x5, // ボックスフィルター(5x5のカーネル)
    };

    // コンストラクタに渡すための鍵
    class ConstructorKey {
    private:
        ConstructorKey() = default;
        friend class PostProcess;
    };

    // passkeyを受け取るコンストラクタ
    explicit PostProcess(ConstructorKey) { }

    // Singleton 取得
    static PostProcess* GetInstance();

    // 共通描画設定
    void PrepareObjectDraw();

    // 初期化
    void Initialize(DirectXCommon* dxcommon);

    // get
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    Camera* GetDefaultCamera() const { return defaultCamera_; }

    // set
    void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
    void SetsrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { this->srvHandle = srvHandle; }

    // モード制御用
    void SetMode(Mode mode) { this->currentMode_ = mode; }
    Mode GetMode() const { return currentMode_; }

    // ImGuiのUIを描画する関数
    void DrawImGui();

    // コピー禁止
    PostProcess(const PostProcess&) = delete;
    PostProcess& operator=(const PostProcess&) = delete;

private:
    friend struct std::default_delete<PostProcess>;

    // コンストラクタ・デストラクタは private
    // CopyRenderTexture() = default;
    ~PostProcess() = default;

private:
    Camera* defaultCamera_ = nullptr;

    // ルートシグネチャの作成
    void RootSignatureInitialize(DirectXCommon* dxcommon);

    // グラフィックスパイプラインの生成
    void graphicsPipelineInitialize(DirectXCommon* dxcommon);

    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGrayscale_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateSepiascale_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateVignette = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFillter = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFillter5x5 = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;

    // 現在のモード
    Mode currentMode_ = Mode::kNormal;
};
