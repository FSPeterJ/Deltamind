#include "Particle_Definitions.hlsl"

//The draw call to this shader should have no input layout elements as the data is in GPU buffers

StructuredBuffer<BillboardParticle> BillboardParticleBuffer : register(t10);
StructuredBuffer<uint> ActiveBillboardParticleIndex : register(t11); 

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
    float size : PSIZE0;
    uint texturedata : TEXCOORD0;
    float rotation : TEXCOORD1;
};

VSOutput main(uint VertexID : SV_VertexID)
{
    VSOutput output = (VSOutput) 0;
    uint particleIndex = VertexID;
    uint sortedIndex = ActiveBillboardParticleIndex[particleIndex]; // Adjust this later when sorting is added
    BillboardParticle BParticle = BillboardParticleBuffer[sortedIndex];

    //This can be optimized by computing the view space data beforehand in a compute shader
    //output.pos = mul(float4(BParticle.position, 1.0f), view);
    output.pos = float4(BParticle.position, 1.0f);
    output.size = BParticle.size; // Temporary, change later
    output.texturedata = BParticle.texturedata;
    output.rotation = BParticle.rotation;
    output.color = BParticle.color;
    return output;
}