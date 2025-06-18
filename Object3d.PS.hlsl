#include "object3d.hlsli"

struct Material
{
    float32_t4 coler;
    int32_t enableLighting;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
    float32_t3 normal : NORMAL0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gMaterial.coler;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = gMaterial.coler * textureColor;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransfomationMatrix.World));

    return output;
}

