#pragma once
#include "DirectXCommon.h"
#include <memory>
class Camera;

class PostProcess {
public:
    struct VignetteData {
        float scale;
        float exponent;
        float padding[2]; // 4バイト×2 = 8バイトの余白を作り、全体で16バイトにする
    };
    struct gIntensity {
        float intensity;
        float padding[3];
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

    // 初期化
    void Initialize(DirectXCommon* dxcommon);

    // 共通描画設定
    void DrawNormal();
    void DrawGrayscale();
    void DrawSepiascale();
    void DrawVignette();
    void DrawHorizontalBlur(bool is5x5); // 引数で3x3か5x5かを判定
    void DrawVerticalBlur(bool is5x5);
    void DrawGaussianFilterHorizontal(bool is5x5); // 今後7x7などを作成予定なのでフラグでないものに変更。
    void DrawGaussianFilterVertical(bool is5x5); // 今後7x7などを作成予定なのでフラグでないものに変更。

    // ImGuiのUIを描画する関数
    void DrawImGui();

public:
    // get
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    Camera* GetDefaultCamera() const { return defaultCamera_; }

    bool IsGrayscale() const { return enableGrayscale_; }
    bool IsSepiascale() const { return enableSepiascale_; }
    bool IsVignette() const { return enableVignette_; }
    VignetteData GetVignetteParam() const { return vignetteParam_; }
    bool IsBoxFilter3x3() const { return enableBoxFilter3x3_; }
    bool IsBoxFilter5x5() const { return enableBoxFilter5x5_; }
    bool IsGaussianFilter3x3() const { return enableGaussianFilter3x3_; }
    bool IsGaussianFilter5x5() const { return enableGaussianFilter5x5_; }

    // set
    void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
    void SetsrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { this->srvHandle = srvHandle; }

    void SetEnableGrayscale(bool enable) { enableGrayscale_ = enable; }
    void SetGrayscaleIntensity(float intensity) { GrayScaleParam_.intensity = intensity; }
    void SetEnableSepiascale(bool enable) { enableSepiascale_ = enable; }
    void SetSepiascaleIntensity(float intensity) { SepiascaleParam_.intensity = intensity; }
    void SetEnableVignette(bool enable) { enableVignette_ = enable; }
    void SetVignetteParam(float scale, float exponent)
    {
        vignetteParam_.scale = scale;
        vignetteParam_.exponent = exponent;
    }
    void SetEnableBoxFilter3x3(bool enable) { enableBoxFilter3x3_ = enable; }
    void SetEnableBoxFilter5x5(bool enable) { enableBoxFilter5x5_ = enable; }
    void SetEnableGaussianFilter3x3(bool enable) { enableGaussianFilter3x3_ = enable; }
    void SetEnableGaussianFilter5x5(bool enable) { enableGaussianFilter5x5_ = enable; }

    void ClearAllEffects()
    {
        enableGrayscale_ = false;
        enableSepiascale_ = false;
        enableVignette_ = false;
        enableBoxFilter3x3_ = false;
        enableBoxFilter5x5_ = false;
        enableGaussianFilter3x3_ = false;
        enableGaussianFilter5x5_ = false;
    }

public:
    // コピー禁止
    PostProcess(const PostProcess&) = delete;
    PostProcess& operator=(const PostProcess&) = delete;

private:
    friend struct std::default_delete<PostProcess>;

    // コンストラクタ・デストラクタは private
    // PostProcess() = default;
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

    /* Grayscale */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGrayscale_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> GrayscaleBuffer_ = nullptr;
    gIntensity* GrayscaleData_ = nullptr;
    gIntensity GrayScaleParam_ = { 1.0f, { 0.0f, 0.0f, 0.0f } };

    /* Sepiascale */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateSepiascale_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> SepiascaleBuffer_ = nullptr;
    gIntensity* SepiascaleData_ = nullptr;
    gIntensity SepiascaleParam_ = { 1.0f, { 0.0f, 0.0f, 0.0f } };

    /* vignette */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateVignette = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vignetteBuffer_ = nullptr;
    VignetteData* vignetteMappedData_ = nullptr;
    VignetteData vignetteParam_ = { 16.0f, 0.8f, { 0.0f, 0.0f } }; // 初期値

    /* BoxFillter3x3 */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterX = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterY = nullptr;

    /* BoxFillter5x5 */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterX5x5 = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterY5x5 = nullptr;

    /* GaussianFilter3x3 */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterX = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterY = nullptr;

    /* GaussianFilter5x5 */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterX5x5 = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterY5x5 = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;

    // 現在のモード
    bool enableGrayscale_ = false;
    bool enableSepiascale_ = false;
    bool enableVignette_ = false;
    bool enableBoxFilter3x3_ = false;
    bool enableBoxFilter5x5_ = false;
    bool enableGaussianFilter3x3_ = false;
    bool enableGaussianFilter5x5_ = false;
};