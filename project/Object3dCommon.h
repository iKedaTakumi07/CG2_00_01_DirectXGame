#pragma once
#include "DirectXCommon.h"

class Object3dCommon {
public:
    // 初期化
    void Initialize(DirectXCommon* dxcommon);

    // 共通描画設定
    void PrepareObjectDraw();

    // get
    DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:
    // ルートシグネチャの作成
    void RootSignatureInitialize(DirectXCommon* dxcommon);

    // グラフィックスパイプラインの生成
    void graphicsPipelineInitialize(DirectXCommon* dxcommon);

    DirectXCommon* dxCommon_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
};
