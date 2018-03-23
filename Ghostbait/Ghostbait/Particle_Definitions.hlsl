//Note: This file is configured to act as header file, but named as hlsl for extension support.

#define ActiveBillboardParticleCounter 0
#define InactiveBillboardParticleCounter 1

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

// I do not know if this places the static variables in a seperate register per CSO
//static uint ActiveBillboardParticleCount = 0;
//static uint InactiveBillboardParticleCount = 0;
//static uint InactiveBillboardParticleCount : register(u1);

RWStructuredBuffer<BillboardParticle> BillboardParticleBuffer : register(u0);


cbuffer InactiveParticles : register(b2)
{
    uint InactiveParticleCount;
    uint IPCpad[3];
};

// The number of alive particles this frame
cbuffer ActiveParticles : register(b3)
{
    uint ActiveParticleCount;
    uint APCpad[3];
};