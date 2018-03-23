#include "Particle_Definitions.hlsl"


cbuffer viewBuffer : register(b0)
{
    float4x4 view;
}


struct VSOutput
{
    float4 pos : SV_POSITION;
    float size : PSIZE0;
    uint textureIndex : TEXCOORD0;
    float rotation : TEXCOORD1;
};

VSOutput main(uint VertexID : SV_VertexID)
{
    VSOutput output = (VSOutput) 0;
    uint particleIndex = VertexID;
    uint sortedIndex = particleIndex; // Adjust this later when sorting is added
    BillboardParticle BParticle = BillboardParticleBuffer[sortedIndex];

    //This can be optimized by computing the view space data beforehand in a compute shader
    output.pos = mul(float4(BParticle.position, 1.0f), view);
    output.size = input.size;
    output.textureIndex = BParticle.compactedData & 0xFF;
    output.rotation = BParticle.rotation;
    return output;
}