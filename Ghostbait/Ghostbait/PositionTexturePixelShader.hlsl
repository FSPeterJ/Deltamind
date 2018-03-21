Texture2D text : register(t0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

SamplerState sample : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    return text.Sample(sample, input.uv);
}