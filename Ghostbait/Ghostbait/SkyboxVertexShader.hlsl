cbuffer ViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer ModelConstantBuffer : register(b1)
{
    matrix model;
};

struct vertexShaderInput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD1;
    uint4 indices : TEXCOORD2;
    float4 weights : BLENDWEIGHTS0;
};

struct outputstruct
{
    float4 pos : SV_POSITION;
    float3 lPos : LOCALPOSITION;
};

outputstruct main(vertexShaderInput input)
{
    outputstruct output;
    output.lPos = input.pos.xyz;
    float4 pos = input.pos;
    float4 norm = float4(input.normal, 0.0f);
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.pos = pos;
    return output;
}