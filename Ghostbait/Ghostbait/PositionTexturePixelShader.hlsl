Texture2D text : register(t0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

SamplerState sample : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 color = text.Sample(sample, input.uv);
   if(!all(color.xyz))
       return float4(0.0f, 0.0f, 0.0f, 0.0f);
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}