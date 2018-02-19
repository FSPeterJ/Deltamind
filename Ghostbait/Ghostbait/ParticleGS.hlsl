cbuffer projectionBuffer : register(b0)
{
    float4x4 projection;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    int tex : TEXCOORD1;
};

struct GSInput
{
    float4 pos : SV_POSITION;
    float size : SIZE;
    int tex : TEXCOORD0;
};
[maxvertexcount(4)]
void main(
	point GSInput input[1],
	inout TriangleStream<GSOutput> output
)
{
    float4 verts[4] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };
    float2 uvs[4];
    verts[0] = float4(-1.0f, 1.0f, 0.0f, 1.0f);
    verts[1] = float4(1.0f, 1.0f, 0.0f, 1.0f);
    verts[2] = float4(-1.0f, -1.0f, 0.0f, 1.0f);
    verts[3] = float4(1.0f, -1.0f, 0.0f, 1.0f);
    uvs[0] = float2(0.0f, 0.0f);
    uvs[1] = float2(1.0f, 0.0f);
    uvs[2] = float2(0.0f, 1.0f);
    uvs[3] = float2(1.0f, 1.0f);

    for (uint i = 0; i < 4; ++i)
    {
        GSOutput element;
        element.pos = verts[i];
        element.uv = uvs[i];
        element.tex = input[0].tex;
        output.Append(element);
    }
}