Texture2DArray textures : register(t0);

SamplerState sample : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    int tex : TEXCOORD1;
};

float4 main(PSInput input) : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
    //return textures.Sample(sample, float3(input.uv, input.tex));
}