Texture2D tex : register(t0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState sample : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 color = tex.Sample(sample, input.uv);
    if(color.w == 0.0f)
        discard;
    return color;
}