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
    float unlitFactor;
};

cbuffer scrollBuffer : register(b2)
{
    float2 offsets;
    float2 padding;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    float2 uv = input.uv + float2(offsets.x * -1.0f, offsets.y);
    float4 diffuseFloat = diffuse.Sample(sample, uv);
    if(diffuseFloat.w == 0.0f)
        discard;
    output.diffuse = float4(diffuseFloat.xyz * diffuseFactor, diffuseFloat.w);
    float4 emissiveFloat = emissive.Sample(sample, uv);
    output.emissive = float4((emissive.Sample(sample, uv) * emissiveFactor).xyz, 1.0f);
    output.normal = float4(((input.norm * 0.5f) + 0.5f), 1.0f);
    output.specular = float4((specular.Sample(sample, uv) * specularFactor).xyz, 1.0f);
    output.depth = float4(input.pos.z, input.pos.w, input.pos.z, 1.0f);
    output.unlit = float4(unlitFactor, unlitFactor, unlitFactor, 1.0f);
    return output;
}