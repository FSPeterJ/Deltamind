Texture2D diffuse : register(t0);
Texture2D specular : register(t1);
Texture2D emissive : register(t2);

SamplerState sample : register(s0);


// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 norm : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
};

struct PixelShaderOutput
{
    float4 diffuse : SV_TARGET0;
    float4 emissive : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 specular : SV_TARGET3;
};

cbuffer factorBuffer : register(b1)
{
    float diffuseFactor;
    float specularFactor;
    float emissiveFactor;
    float morepadding;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.diffuse = diffuse.Sample(sample, input.uv) * diffuseFactor;
    output.emissive = emissive.Sample(sample, input.uv) * emissiveFactor;
    output.normal = float4(input.norm, 1.0f);
    output.specular = specular.Sample(sample, input.uv) * specularFactor;
    return output;
}