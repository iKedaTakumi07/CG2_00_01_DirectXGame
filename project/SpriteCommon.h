#pragma once
#include "DirectXCommon.h"
#include "wrl.h"

class SpriteCommon {
public:
    // Singleton 取得
    static SpriteCommon* GetInstance();

    // 初期化
    void Initialize(DirectXCommon* dxcommon);

    DirectXCommon* GetDxCommon() const { return dxCommon_; }

    // 共通描画設定
    void PrepareSpriteDraw();

    SpriteCommon(const SpriteCommon&) = delete;
    SpriteCommon& operator=(const SpriteCommon&) = delete;

private:
    SpriteCommon() = default;
    ~SpriteCommon() = default;

private:
    // ルートシグネチャの作成
    void RootSignatureInitialize(DirectXCommon* dxcommon);

    // グラフィックスパイプラインの生成
    void graphicsPipelineInitialize(DirectXCommon* dxcommon);

    DirectXCommon* dxCommon_;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
};
