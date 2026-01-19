#pragma once
#include "DirectXCommon.h"

class ModelCommon {
public:
    // Singleton 取得
    static ModelCommon* GetInstance();


    void Initialize(DirectXCommon* dxCommon);

    ModelCommon(const ModelCommon&) = delete;
    ModelCommon& operator=(const ModelCommon&) = delete;
    DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:
    ModelCommon() = default;
    ~ModelCommon() = default;


private:
    DirectXCommon* dxCommon_;
};
