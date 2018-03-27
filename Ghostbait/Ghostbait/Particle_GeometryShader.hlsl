cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer TextureArrayDimensionsConstantBuffer : register(b1)
{
    uint maxU;
    uint maxV;
    uint padding[2];
};


struct GSOutput
{
    float4 pos : SV_POSITION;
    float3 uvw : TEXCOORD0;
};

struct GSInput
{
    float4 pos : SV_POSITION;
    float size : PSIZE0;
    uint texturedata : TEXCOORD0;
    float rotation : TEXCOORD1;
};

//This is to determine the end of the texture UV in relation to the texturearray size
float2 calculateUVOffset(uint texturedata)
{
    return float2((float) (texturedata & (0xFFF << 20)) / (float) maxU, (float) (texturedata & (0xFFF << 8)) / (float) maxU);
}


[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{
    
    float4 verts[4] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };
    float3 uvws[4];
    verts[0] = float4(input[0].pos.x - input[0].size, input[0].pos.y + input[0].size, input[0].pos.z, 1.0f);
    verts[1] = float4(input[0].pos.x + input[0].size, input[0].pos.y + input[0].size, input[0].pos.z, 1.0f);
    verts[2] = float4(input[0].pos.x - input[0].size, input[0].pos.y - input[0].size, input[0].pos.z, 1.0f);
    verts[3] = float4(input[0].pos.x + input[0].size, input[0].pos.y - input[0].size, input[0].pos.z, 1.0f);

    float2 OffsetUV = calculateUVOffset(input[0].texturedata);
    float w = input[0].texturedata & 0xFF;
    uvws[0] = float3(0.0f, 0.0f, w);
    uvws[1] = float3(OffsetUV.x, 0.0f, w);
    uvws[2] = float3(0.0f, OffsetUV.y, w);
    uvws[3] = float3(OffsetUV.x, OffsetUV.y, w);

    GSOutput element = (GSOutput) 0;
    for (uint i = 0; i < 4; ++i)
    {
        element.pos = mul(verts[i], view);
        element.pos = mul(element.pos, projection);
        element.uvw = uvws[i];
        output.Append(element);
    }
    //Will probably need later for draw instanced indexed
    //output.RestartStrip(); //

}

