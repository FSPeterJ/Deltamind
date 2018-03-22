#include "Particle_Definitions.hlsl"
#include "Global.hlsl"


RWStructuredBuffer<BillboardParticle> BillboardParticleBuffer : register(u0);

RWStructuredBuffer<uint> ActiveBillboardParticleIndex : register(u1);
AppendStructuredBuffer<uint> InactiveBillboardParticleIndex : register(u2);

groupshared uint BillboardParticleCount;


static uint ActiveBillboardParticles;

[numthreads(256, 1, 1)]
void CS_Simulate(uint3 DThreadID : SV_DispatchThreadID)
{
    if (DThreadID.x == 0)
    {
        //This is stupid.
        BillboardParticleCount = BillboardParticleBuffer.IncrementCounter();
        BillboardParticleBuffer.DecrementCounter();

    }
    GroupMemoryBarrierWithGroupSync();


        //if (ActiveBillboardParticleIndex[DThreadID.x] < ActiveBillboardParticleIndex.GetDimensions)
        //{

        //}

}