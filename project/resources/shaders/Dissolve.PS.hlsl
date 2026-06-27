#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);
cbuffer DessolveParameta : register(b0)
{
    float32_t4 thresholdcolor;
    float32_t3 Edegcolor;
    float32_t gthreshold;
    float32_t edgeWidth;
};


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    PixelShaderOutput output;
    
  
    
    
    if (mask <= gthreshold)
    {
        output.color = thresholdcolor;
    }
    else
    {
        // Edeg
        float32_t edgeWidth = 0.03f;
        float32_t edge = 1.0f - smoothstep(gthreshold, gthreshold + edgeWidth, mask);
        
        output.color = gTexture.Sample(gSampler, input.texcoord);
        // Edegポイ程指定した色を加算
        output.color.rgb += edge * Edegcolor;
    }
    

    return output;
}