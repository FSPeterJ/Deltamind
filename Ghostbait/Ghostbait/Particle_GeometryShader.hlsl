#include "Global.hlsl"

cbuffer ModelViewProjectionConstantBuffer : register(b2)
{
    matrix view;
    matrix projection;
};


cbuffer TextureArrayDimensionsConstantBuffer : register(b1)
{
    uint maxU;
    uint maxV;
    uint paddingtex[2];
};

struct genericLight
{
    float4 color;
    float3 dir;
    float radius;
    float3 pos;
    float outerRadius;
};


struct GSOutput
{
    float4 pos : SV_POSITION;
    float3 uvw : TEXCOORD0;
    float4 color : COLOR0;
};

struct GSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float size : PSIZE0;
    uint texturedata : TEXCOORD0;
    float rotation : TEXCOORD1;
};

//This is to determine the end of the texture UV in relation to the texturearray size
float2 calculateUVOffset(uint texturedata)
{
    return float2((float) (texturedata >> 20) / (float) maxU, (float) ((texturedata & (0xFFF << 8)) >> 8) / (float) maxV);
}


[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{

    float2 OffsetUV = calculateUVOffset(input[0].texturedata);
    float w = input[0].texturedata & 0xFF;

    float3 planeNormal = CameraCenterpoint - (float3) input[0].pos;

    planeNormal = normalize(planeNormal);

    // TODO: To prevent pinwheeling when at Y min or max relative to the player possibly use Up as purely relative to the player's viewspace up
    // Everspace suffers from this graphical error on it's nebula cloud billboards and Reveation Online on it's tree leaves which may look very bizzare
    // For now, up is relative to world up
    float3 upVector = float3(0.0f, 1.0f, 0.0f);
    float4 rightVector = float4(normalize(cross(upVector, planeNormal)), 0.0f);
    //upVector = normalize(cross(planeNormal, (float3) rightVector));
    //rightVector *= input[0].size;

    float s, c;
    sincos(input[0].rotation, s, c);
    float3 upVectorRotated = c * rightVector - s * upVector;
    float3 rightVectorRotated = s * rightVector + c * upVector;
    upVectorRotated = normalize(cross(planeNormal, (float3) rightVectorRotated));
    rightVectorRotated *= input[0].size;
    upVectorRotated *= input[0].size;
    
    GSOutput element = (GSOutput) 0;
    
    element.pos = input[0].pos - float4(rightVectorRotated, 0) - float4(upVectorRotated, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(projection, element.pos );
    element.uvw = float3(0.0f, 0.0f, w);
    element.color = input[0].color;
    output.Append(element);
    
    element.pos = input[0].pos + float4(rightVectorRotated, 0) - float4(upVectorRotated, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(projection, element.pos );
    element.uvw = float3(OffsetUV.x, 0.0f, w);
    element.color = input[0].color;
    output.Append(element);
    
    element.pos = input[0].pos - float4(rightVectorRotated, 0) + float4(upVectorRotated, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(projection, element.pos);
    element.uvw = float3(0.0f, OffsetUV.y, w);
    element.color = input[0].color;
    output.Append(element);
    
    element.pos = input[0].pos + float4(rightVectorRotated, 0) + float4(upVectorRotated, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(projection, element.pos );
    element.uvw = float3(OffsetUV.x, OffsetUV.y, w);
    element.color = input[0].color;
    output.Append(element);


}







