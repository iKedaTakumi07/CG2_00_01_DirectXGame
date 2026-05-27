#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 usStepSize = float32_t2(rcp(width), 0.0f);
    
    PixelShaderOutput output;
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    // 横方向（X軸）のみループ
    for (int x = -2; x <= 2; ++x)
    {
        float2 offset = float2(x * usStepSize.x, 0.0f);
        // 5ピクセルなので重みは 1.0 / 5.0 = 0.2f
        output.color.rgb += gTexture.Sample(gSampler, input.texcoord + offset).rgb * (1.0f / 5.0f);
    }

    return output;
}