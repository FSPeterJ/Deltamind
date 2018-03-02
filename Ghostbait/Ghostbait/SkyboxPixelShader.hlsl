TextureCube sky : register(t0);

SamplerState skySample : register(s0);

struct pixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 lpos : LOCALPOSITION;
};

float4 main(pixelShaderInput input) : SV_TARGET
{
    return sky.Sample(skySample, input.lpos);
}