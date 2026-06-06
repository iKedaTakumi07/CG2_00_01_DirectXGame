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

    struct FilterData {
        int32_t kernelSize; // 3, 5, 7 など
        float sigma; // ガウシアンフィルター用の標準偏差（Boxでは未使用）
        float padding[2]; // 16バイトアライメント調整用
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
    void DrawBoxFilterHorizontal();
    void DrawBoxFilterVertical();
    void DrawGaussianFilterHorizontal();
    void DrawGaussianFilterVertical();
    void DrawOutLine();

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
    bool IsBoxFilter() const { return enableBoxFilter_; }
    bool IsGaussianFilter() const { return enableGaussianFilter_; }
    bool IsOutLine() const { return enableOutLine_; }

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

    void SetEnableBoxFilter(bool enable) { enableBoxFilter_ = enable; }
    void SetKernelSizeBoxFilter(int KernelSize) { boxKernelSize_ = KernelSize; }

    void SetEnableGaussianFilter(bool enable) { enableGaussianFilter_ = enable; }
    void SetKernelSizeGaussianFilter(int KernelSize) { gaussianKernelSize_ = KernelSize; }
    void SetSigmaGaussianFilter(float Sigma) { gaussianSigma_ = Sigma; }

    void SetEnableOutLine(bool enable) { enableOutLine_ = enable; }

    void ClearAllEffects()
    {
        enableGrayscale_ = false;
        enableSepiascale_ = false;
        enableVignette_ = false;
        enableBoxFilter_ = false;
        enableGaussianFilter_ = false;
        enableOutLine_ = false;
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

    /* BoxFillter */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterX = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateBoxFilterY = nullptr;

    /* GaussianFilter */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterX = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterY = nullptr;

    // Filter用
    Microsoft::WRL::ComPtr<ID3D12Resource> filterBuffer_ = nullptr;
    FilterData* filterMappedData_ = nullptr;
    FilterData filterParam_ = { 3, 2.0f, { 0.0f, 0.0f } };

    int boxKernelSize_ = 3;
    int gaussianKernelSize_ = 3;
    float gaussianSigma_ = 2.0f;

    /* OutLine */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateOutLine = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;

    // 現在のモード
    bool enableGrayscale_ = false;
    bool enableSepiascale_ = false;
    bool enableVignette_ = false;
    bool enableBoxFilter_ = false;
    bool enableGaussianFilter_ = false;
    bool enableOutLine_ = false;
};