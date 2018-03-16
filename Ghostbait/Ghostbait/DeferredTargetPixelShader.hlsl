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
    float4 depth : SV_TARGET4;
    float4 unlit : SV_TARGET5;
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
    float4 diffuseFloat = diffuse.Sample(sample, input.uv);
    output.diffuse = float4(diffuseFloat.xyz * diffuseFactor, diffuseFloat.w);
    output.emissive = emissive.Sample(sample, input.uv) * emissiveFactor;
    output.normal = float4(((input.norm * 0.5f) + 0.5f), 1.0f);
    output.specular = float4((specular.Sample(sample, input.uv) * specularFactor).xyz, 1.0f);
    output.depth = float4(input.pos.z, input.pos.z, input.pos.z, 1.0f);
    output.unlit = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}