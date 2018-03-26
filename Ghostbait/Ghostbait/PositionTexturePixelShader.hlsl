Texture2D text : register(t0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer colorBuffer : register(b3)
{
    float4 textColor;
};

SamplerState sample : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 color = text.Sample(sample, saturate(input.uv));
   if(!all(color.xyz))
        discard;
    return textColor;
}