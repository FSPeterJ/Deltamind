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
    


    float2 OffsetUV = calculateUVOffset(input[0].texturedata);
    float w = input[0].texturedata & 0xFF;




    float3 planeNormal = cameraPos - (float3)input[0].pos;

    planeNormal = normalize(planeNormal);

    float3 upVector = float3(0.0f, 1.0f, 0.0f);
    float4 rightVector = float4(normalize(cross(upVector, planeNormal)), 0.0f);
    upVector = normalize(cross(planeNormal, rightVector));
    rightVector *= input[0].size;

    upVector *= input[0].size;
    
    GSOutput element = (GSOutput) 0;
    
    element.pos = input[0].pos - rightVector - float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(0.0f, 0.0f, w);
    output.Append(element);
    
    element.pos = input[0].pos + rightVector - float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(OffsetUV.x, 0.0f, w);
    output.Append(element);
    
    element.pos = input[0].pos - rightVector + float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(0.0f, OffsetUV.y, w);
    output.Append(element);
    
    element.pos = input[0].pos + rightVector + float4(upVector, 0);
    element.pos = mul(element.pos, view);
    element.pos = mul(element.pos, projection);
    element.uvw = float3(OffsetUV.x, OffsetUV.y, w);
    output.Append(element);

    //GSOutput element = (GSOutput) 0;
    //for (uint i = 0; i < 4; ++i)
    //{
        
    //    element.uvw = uvws[i];
    //    output.Append(element);
    //}
    //Will probably need later for draw instanced indexed
    //output.RestartStrip(); //

}










//[maxvertexcount(4)]
//void main(point GSInput input[1], inout TriangleStream<GSOutput> output)
//{
//    float4 verts[4] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };

//    float2 OffsetUV = calculateUVOffset(input[0].texturedata);
//    float w = input[0].texturedata & 0xFF;

//    float3 planeNormal = (float3) input[0].pos - cameraPos;
//    planeNormal = normalize(planeNormal);

//    float3 upVector = float3(0.0f, 1.0f, 0.0f);
//    float4 rightVector = float4(normalize(cross(planeNormal, upVector)), 0.0f);

//    rightVector = rightVector * input[0].size;
//    upVector = float3(0, input[0].size, 0); // The billboard is straight up and down

//    float3 toCamera = normalize(cameraPos - (float3) input[0].pos);
//    float3 up = float3(0.0, 1.0, 0.0);
//    float3 left = cross(up, toCamera);
//    float3 Pos = input[0].pos;

//    GSOutput element = (GSOutput) 0;

//    element.pos = input[0].pos - rightVector;
//    element.uvw = float3(0.0f, 0.0f, w);
//    output.Append(element);

//    element.pos = input[0].pos + rightVector;
//    element.uvw = float3(OffsetUV.x, 0.0f, w);
//    output.Append(element);

//    element.pos = input[0].pos - rightVector + float4(upVector, 0);
//    element.uvw = float3(0.0f, OffsetUV.y, w);
//    output.Append(element);

//    element.pos = input[0].pos + rightVector + float4(upVector, 0);
//    element.uvw = float3(OffsetUV.x, OffsetUV.y, w);
//    output.Append(element);

//    //Will probably need later for draw instanced indexed
//    //output.RestartStrip(); //
//}


