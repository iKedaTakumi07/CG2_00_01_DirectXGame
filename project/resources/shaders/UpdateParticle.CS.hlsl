struct Particle
{
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};
struct PerFrame
{
    float32_t time;
    float32_t deltaTime;
};

static const uint32_t kMaxParticles = 1024;
ConstantBuffer<PerFrame> gPerFrame : register(b0);
RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<int32_t> gFreeList : register(u2);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        // alphaが0のparticleは死んでいるとみなして更新しない
        if (gParticles[particleIndex].color.a != 0)
        {
            // スケールに0を入れておいてvertexshader出力で忘却されるようにする
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float32_t alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
        
        // alphaが0なのでFreeにする
        if (gParticles[particleIndex].color.a == 0)
        {
            // スケールに0を入れておいてvertexshader出力で忘却されるようにする
            gParticles[particleIndex].scale = float32_t3(0.0f, 0.0f, 0.0f);
            int32_t freeListIndex;
            InterlockedAnd(gFreeListIndex[0], 1, freeListIndex);
            // 最新のfreeListindexの場所に死んだParticleのindexを設定する
            if ((freeListIndex + 1) < kMaxParticles)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                // 通るはずがない、万が一の安全策。
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}