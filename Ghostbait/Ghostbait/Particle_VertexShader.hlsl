#include "Particle_Definitions.hlsl"

//The draw call to this shader should have no input layout elements as the data is in GPU buffers

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};


struct VSOutput
{
    float4 pos : SV_POSITION;
    float size : PSIZE0;
    uint texturedata : TEXCOORD0;
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
    output.size = BParticle.endSize; // Temporary, change later
    output.texturedata = BParticle.texturedata & 0xFF;
    output.rotation = BParticle.rotation;
    return output;
}