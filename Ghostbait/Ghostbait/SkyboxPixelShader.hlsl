TextureCube sky : register(t0);

SamplerState skySample : register(s0);

struct pixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 lpos : LOCALPOSITION;
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

PixelShaderOutput main(pixelShaderInput input)
{
    PixelShaderOutput output;
    output.diffuse = sky.Sample(skySample, input.lpos);
    output.emissive = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.normal = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.depth = float4(input.pos.z, input.pos.z, input.pos.z, 1.0f);
    output.unlit = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}