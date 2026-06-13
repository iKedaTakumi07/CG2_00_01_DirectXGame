#pragma once
#include "DirectXCommon.h"
#include "Math.h"
#include <memory>
class Camera;
class OffscreenSurface;

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

    struct BlurData {
        Vector2 kCenter; // 中心点
        float kBlurwidth; // ぼかしの幅
        float padding[1];
    };

    // アウトライン用
    struct LuminanceOutlineData {
        float weightMultiplier;
        float padding[3];
    };
    struct DepthOutlineData {
        Matrix4x4 projectionInverse;
        float weightMultiplier;
        float padding[3];
    };

    enum class EffectType {
        Grayscale,
        Sepiascale,
        DepthOutline,
        LuminanceOutline,
        BoxFilter,
        GaussianFilter,
        RadialBlur,
        Vignette,
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

    // 更新
    void Update();

    // 外部からエフェクトを一斉実行
    void Execute(OffscreenSurface* surfaceA, OffscreenSurface* surfaceB);

    // 共通描画設定
    void DrawNormal();
    void DrawGrayscale();
    void DrawSepiascale();
    void DrawVignette();
    void DrawBoxFilterHorizontal();
    void DrawBoxFilterVertical();
    void DrawGaussianFilterHorizontal();
    void DrawGaussianFilterVertical();
    void DrawLuminanceOutLine();
    void DrawDepthOutLine();
    void DrawRadialBlur();

    // ImGuiのUIを描画する関数
    void DrawImGui();

public:
    // get //

    DirectXCommon* GetDxCommon() const { return dxCommon_; }
    Camera* GetDefaultCamera() const { return defaultCamera_; }

    bool IsGrayscale() const { return enableGrayscale_; } // グレイスケール
    bool IsSepiascale() const { return enableSepiascale_; } // セピアスケール
    bool IsVignette() const { return enableVignette_; } // ヴィネッティング
    VignetteData GetVignetteParam() const { return vignetteParam_; }
    bool IsBoxFilter() const { return enableBoxFilter_; } // ボックスフィルター
    bool IsGaussianFilter() const { return enableGaussianFilter_; } // ガウシアンフィルタ
    bool IsLuminanceOutLine() const { return enableLuminanceOutLine_; } // 輝度アウトライン
    bool IstDepthOutLine() const { return enableDepthOutLine_; } // Depthアウトライン
    bool IsRadialBlur() const { return enableRadialBlur_; } // ラジアルブラー

    // set
    void SetDepthSrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) { this->depthSrvHandle = handle; }

    void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
    void SetsrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle) { this->srvHandle = srvHandle; }

    void SetEnableGrayscale(bool enable) { enableGrayscale_ = enable; }
    void SetGrayscaleIntensity(float intensity) { GrayScaleParam_.intensity = intensity; } // グレイスケールパラメーターセット

    void SetEnableSepiascale(bool enable) { enableSepiascale_ = enable; }
    void SetSepiascaleIntensity(float intensity) { SepiascaleParam_.intensity = intensity; } // セピアスケールパラメーターセット

    void SetEnableVignette(bool enable) { enableVignette_ = enable; }
    void SetVignetteParam(float scale, float exponent) // ヴィネッティングパラメーターセット
    {
        vignetteParam_.scale = scale;
        vignetteParam_.exponent = exponent;
    }

    void SetEnableBoxFilter(bool enable) { enableBoxFilter_ = enable; }
    void SetKernelSizeBoxFilter(int KernelSize) // ボックスフィルターパラメーターセット
    {
        boxKernelSize_ = KernelSize;
        if (boxKernelSize_ % 2 == 0)
            boxKernelSize_++;
        if (boxKernelSize_ >= 31) // オーバーフロー&&過度なエフェクト対策
            boxKernelSize_ = 31;
    }

    void SetEnableGaussianFilter(bool enable) { enableGaussianFilter_ = enable; }
    void SetKernelSizeGaussianFilter(int KernelSize) // ガウシアンフィルタカーネルサイズセット
    {
        gaussianKernelSize_ = KernelSize;
        if (gaussianKernelSize_ % 2 == 0)
            gaussianKernelSize_++;
        if (gaussianKernelSize_ >= 31) // オーバーフロー&&過度なエフェクト対策
            gaussianKernelSize_ = 31;
    };
    void SetSigmaGaussianFilter(float Sigma) { gaussianSigma_ = Sigma; } // ガウシアンフィルタぼかし強度

    void SetEnableLuminanceOutLine(bool enable) { enableLuminanceOutLine_ = enable; }
    void SetLuminanceOutlineWeight(float weight) { LuminanceParam.weightMultiplier = weight; } // 輝度アウトラインパラメーターセット

    void SetDepthOutLine(bool enable) { enableDepthOutLine_ = enable; }
    void SetDepthOutlineWeight(float weight) { weightMultiplierParam = weight; } // Depthアウトラインパラメーターセット

    void SetRadialBlur(bool enabel) { enableRadialBlur_ = enabel; }
    void SetRadialBlurParam(Vector2 pos, float kBlurwidth) // ラジアルブラーパラメーターセット
    {
        RadialBlurParam.kCenter = pos;
        RadialBlurParam.kBlurwidth = kBlurwidth;
    }

    void ClearAllEffects() // エフェクト全リセット
    {
        enableGrayscale_ = false;
        enableSepiascale_ = false;
        enableVignette_ = false;
        enableBoxFilter_ = false;
        enableGaussianFilter_ = false;
        enableLuminanceOutLine_ = false;
        enableDepthOutLine_ = false;
        enableRadialBlur_ = false;
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
    Microsoft::WRL::ComPtr<ID3D12Resource> boxFilterBuffer_ = nullptr;
    FilterData* boxFilterMappedData_ = nullptr;
    FilterData boxFilterParam_;

    /* GaussianFilter */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterX = nullptr;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateGaussianFilterY = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> gaussianFilterBuffer_ = nullptr;
    FilterData* gaussianFilterMappedData_ = nullptr;
    FilterData gaussianFilterParam_;

    // Filter用
    int boxKernelSize_ = 3;
    int gaussianKernelSize_ = 3;
    float gaussianSigma_ = 2.0f;

    /* OutLine(輝度) */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateLuminanceOutLine = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> LuminanceBuffer_ = nullptr;
    LuminanceOutlineData* LuminanceData_ = nullptr;
    LuminanceOutlineData LuminanceParam = { 6.0f, { 0.0f, 0.0f, 0.0f } };

    /* OutLine(Depth) */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateDepthOutLine = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> outlineBuffer_ = nullptr;
    DepthOutlineData* outlineMappedData_ = nullptr;
    float weightMultiplierParam = 1.0f;

    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE depthSrvHandle;

    /* RadialBlur */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateRadialBlur = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> RadialBlurBuffer_ = nullptr;
    BlurData* RadialBlurData_ = nullptr;
    BlurData RadialBlurParam = { { 0.5f, 0.5f }, 0.01f, { 0.0f } };

    // 現在のモード
    bool enableGrayscale_ = false;
    bool enableSepiascale_ = false;
    bool enableVignette_ = false;
    bool enableBoxFilter_ = false;
    bool enableGaussianFilter_ = false;
    bool enableLuminanceOutLine_ = false;
    bool enableDepthOutLine_ = false;
    bool enableRadialBlur_ = false;

    // エフェクトの描画順を管理するためのリスト
    std::vector<EffectType> effectOrder_;
};