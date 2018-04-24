cbuffer ModelViewProjectionConstantBuffer : register(b0)
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

cbuffer lightBuffer : register(b2)
{
    genericLight lights[83];
    float3 ambientColor;
    float ambientIntensity;
    float3 cameraPos;
    uint paddinglights[1];
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
    float2 temp = float2((float) (texturedata >> 20), (float) ((texturedata & (0xFFF << 8)) >> 8));
    temp.x /= (float)maxU;
    temp.y /= (float)maxV;
    return temp;
}


[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
{

    float2 OffsetUV = calculateUVOffset(input[0].texturedata);
    float w = input[0].texturedata & 0xFF;

    float3 planeNormal = cameraPos - (float3)input[0].pos;

    planeNormal = normalize(planeNormal);

    float3 upVector = float3(0.0f, 1.0f, 0.0f);
    float4 rightVector = float4(normalize(cross(upVector, planeNormal)), 0.0f);
    upVector = normalize(cross(planeNormal, (float3) rightVector));
    rightVector *= input[0].size;

    upVector *= input[0].size;
    
    GSOutput element = (GSOutput) 0;
    
    element.pos = input[0].pos - rightVector - float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(0.0f, 0.0f, w);
    element.color = input[0].color;
    output.Append(element);
    
    element.pos = input[0].pos + rightVector - float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(OffsetUV.x, 0.0f, w);
    element.color = input[0].color;
    output.Append(element);
    
    element.pos = input[0].pos - rightVector + float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(0.0f, OffsetUV.y, w);
    element.color = input[0].color;
    output.Append(element);
    
    element.pos = input[0].pos + rightVector + float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(OffsetUV.x, OffsetUV.y, w);
    element.color = input[0].color;
    output.Append(element);

    //Will probably need later for draw instanced indexed?
    //output.RestartStrip(); //

}







