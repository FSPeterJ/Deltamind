struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

[maxvertexcount(4)]
void main(
	point float4 input[1] : SV_POSITION,
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
        output.Append(element);
    }
}