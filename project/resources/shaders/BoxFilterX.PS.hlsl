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
    for (int x = -1; x <= 1; ++x)
    {
        float2 offset = float2(x * usStepSize.x, 0.0f);
        // 3ピクセルなので重みは 1.0 / 3.0 = 0.33…f
        output.color.rgb += gTexture.Sample(gSampler, input.texcoord + offset).rgb * (1.0f / 3.0f);
    }

    return output;
}