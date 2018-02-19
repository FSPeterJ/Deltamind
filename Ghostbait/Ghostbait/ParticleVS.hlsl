cbuffer viewBuffer : register(b0)
{
    float4x4 view;
}

struct VSInput
{
    float3 pos : POSITION;
    float size : SIZE;
    int tex : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float size : SIZE;
    int tex : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(float4(input.pos, 1.0f), view);
    output.size = input.size;
    output.tex = input.tex;
    return output;
}