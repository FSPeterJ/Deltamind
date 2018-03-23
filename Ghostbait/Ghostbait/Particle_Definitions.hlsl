//Note: This file is configured to act as header file, but named as hlsl for extension support.

struct BillboardParticle // (48 bytes) 500,000  = 25MB of VRAM for particle data (not including textures)
{
     //Dynamic data  (16 bytes)
    float3 position;
    float rotation;
    
    //Dynamic data  (16 bytes)
    float3 velocity;
    float age;

    //Constant data (16 bytes)
    float lifespan; // how long the particle will last
    float startSize; //particle scale over time data
    float endSize;
    uint compactedData; // 16 bits - no data | 8 bits - emitter index (debug data) | 8 bits - texture index  - All subject to change in the future?
};

static uint ActiveBillboardParticleCount = 0;
static uint InactiveBillboardParticleCount = 0;
RWStructuredBuffer<BillboardParticle> BillboardParticleBuffer : register(u0);

