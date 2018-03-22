cbuffer screenBuffer : register(b3)
{
    float height;
    float width;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
    output.uv = input.uv;
    output.pos.x = -1.0f + ((input.pos.x / width) * 2.0f);
    output.pos.y = 1.0f - ((input.pos.y / height) * 2.0f);
    output.pos.z = 0.0f;
    output.pos.w = 1.0f;
	return output;
}