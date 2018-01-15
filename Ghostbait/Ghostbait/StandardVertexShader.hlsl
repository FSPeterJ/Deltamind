cbuffer ViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer ModelConstantBuffer : register(b1)
{
    matrix model;
};

cbuffer animdataBuffer : register(b2)
{
    matrix joints[50];
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
    float2 uv : TEXCOORD0;
};

outputstruct main(vertexShaderInput input)
{
    outputstruct output;
    float4 pos = input.pos;
    float4 skinned_pos = { 0, 0, 0, 0 };
    for (int i = 0; i < 4; ++i)
        skinned_pos += mul(joints[input.indices[i]], pos) * input.weights[i];

    float4 outputpos = pos;
    float4 norm = float4(input.normal, 0.0f);
    outputpos = mul(outputpos, model);
    outputpos = mul(outputpos, view);
    outputpos = mul(outputpos, projection);
    output.pos = outputpos;

    output.uv = input.uv;
	
    norm = mul(norm, model);
    norm = normalize(norm);

    return output;
}