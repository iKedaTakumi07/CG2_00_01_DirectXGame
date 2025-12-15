#include "ParticleEmitter.h"
#include "ParticleManager.h"

ParticleEmitter::ParticleEmitter(const std::string& groupName, const Transform& transform, float emitRate, uint32_t emitCount)
    : groupName_(groupName)
    , transform_(transform)
    , emitRate_(emitRate)
    , emitCount_(emitCount)
    , elapsedTime_(0.0f)
{
}

void ParticleEmitter::Update()
{
    if (emitRate_ <= 0.0f) {
        return;
    }

    // 時刻を進める
    elapsedTime_ += 1.0f/60.0f;

    // 1回発生するのに必要な時間
    const float emitInterval = 1.0f / emitRate_;

    // 発生可能な回数を計算（余剰時間を保持）
    while (elapsedTime_ >= emitInterval) {

        ParticleManager::getInstance()->Emit(groupName_, transform_.translate, emitCount_);

        elapsedTime_ -= emitInterval;
    }
}

void ParticleEmitter::Emit()
{
    ParticleManager::getInstance()->Emit("pori", transform_.translate, 3);
}
