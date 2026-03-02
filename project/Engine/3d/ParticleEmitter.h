#pragma once
#include "../base/Math.h"
#include <string>

class ParticleEmitter {
public:
    // コンストラクタ
    ParticleEmitter(
        const std::string& groupName,
        const Transform& transform,
        float emitRate, // 1秒あたり何回 Emit するか
        uint32_t emitCount // 1回の Emit で何個出すか
    );

    // 更新
    void Update();

    void Emit();

    // Setter
    void SetTransform(const Transform& transform)
    {
        transform_ = transform;
    }

private:
    std::string groupName_; // ParticleGroup 名
    Transform transform_; // エミッタの位置

    float emitRate_ = 0.0f; // 発生頻度（回/秒）
    uint32_t emitCount_ = 0; // 1回の発生数

    float elapsedTime_ = 0.0f; // 経過時間
};