#pragma once
#include "DirectXCommon.h"
#include <memory>
class Camera;

class PostProcess {
public:
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

   D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
};
