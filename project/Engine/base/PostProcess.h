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
        kBoxFilterSeparable3x3, // ボックスフィルター(3x3の分離可能フィルタ)
        kBoxFilterSeparable5x5, // ボックスフィルター(5x5の分離可能フィルタ)
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
    void DrawHorizontalBlur(); // 横ぼかし用
    void DrawVerticalBlur(); // 縦ぼかし用

    // 初期化
    void Initialize(DirectXCommon* dxcommon);

    // get
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    Camera* GetDefaultCamera() const { return defaultCamera_; }

    // set
    void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
    void SetsrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { this->srvHandle = srvHandle; }
    Mode GetMode() const { return currentMode_; }

    // モード制御用
    void SetMode(Mode mode) { this->currentMode_ = mode; }

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

    /* BoxFillter3x3 */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFillterX = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFillterY = nullptr;

    /* BoxFillter5x5 */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterX5x5 = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterY5x5 = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;

    // 現在のモード
    Mode currentMode_ = Mode::kNormal;
};