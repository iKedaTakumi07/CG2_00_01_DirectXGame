#pragma once
#include "DirectXCommon.h"

class Object3dCommon {
public:
    // 初期化
    void Initialize(DirectXCommon* dxcommon);

private:
    // ルートシグネチャの作成
    void RootSignatureInitialize(DirectXCommon* dxcommon);

    // グラフィックスパイプラインの生成
    void graphicsPipelineInitialize(DirectXCommon* dxcommon);

    DirectXCommon dxCommon_;
};
