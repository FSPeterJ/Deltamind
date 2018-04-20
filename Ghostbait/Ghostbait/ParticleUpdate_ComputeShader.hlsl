#include "Particle_Definitions.hlsl"
#include "Global.hlsl"


RWStructuredBuffer<Particle> ParticleBuffer : register(u0);

RWStructuredBuffer<uint> ActiveParticleIndex : register(u1);
RWStructuredBuffer<uint> InactiveParticleIndex : register(u2);
RWBuffer<uint> DrawArgs : register(u3);
AppendStructuredBuffer<float2> SortingData : register(u4);


//groupshared uint ActiveParticleLastIndex;
//groupshared uint OriginalActiveBillboardParticleCount;
//groupshared uint InactiveBillboardParticleCount;
//groupshared uint OriginalInactiveBillboardParticleCount;

[numthreads(256, 1, 1)]
void main(uint3 DThreadID : SV_DispatchThreadID)
{
    //if (DThreadID.x == 0)
    //{
    //    DrawArgs[0] = 1;

    //}
    //GroupMemoryBarrierWithGroupSync();

    //The reason behind this system is to quit out of cycling particles early.  From what I understand that way that GPU thread groups work
    //is they are processed in batches.  This means if 64 threads are assigned a task and only 2 of them are active, the entire thread group must wait 
    //until all have processed in order to release the batch.

    if (DThreadID.x < ActiveParticleCount)
    {
        uint particleBufferIndex = ActiveParticleIndex[DThreadID.x];
        Particle particle = ParticleBuffer[particleBufferIndex];


        particle.age -= FrameTime;

        if (particle.age > 0.0f)
        {
            float scaledLife = 1.0 - saturate(particle.age / particle.lifespan);
            particle.color = lerp(particle.startColor, particle.endColor, scaledLife);
            particle.size = lerp(particle.startSize, particle.endSize, scaledLife);

            //Possible Additions: variable rotation speed, acceleration
            //Bparticle.m_Rotation += 0.24 * FrameTime;
            float3 NewPosition = particle.position;
            NewPosition += particle.velocity * FrameTime;
            particle.position = NewPosition;
            particle.distanceToCamera = length(NewPosition - CameraCenterpoint.xyz);


            ParticleBuffer[particleBufferIndex] = particle;
            SortingData.Append(float2(particle.distanceToCamera, asfloat(particleBufferIndex)));
        }
        else
        {
            //InterlockedAdd(DrawArgs[1], -1);// Wait, can't we use copy subresource count to this buffer?
            //Bparticle.age = -1;
            //Swap
            //BillboardParticleBuffer[particleIndex] = Bparticle; // There is no need to spend time writing data of a dead particle
            //uint Index = ActiveBillboardParticleIndex.DecrementCounter();
            ActiveParticleIndex.DecrementCounter();
            //uint tempNotUsed;
            //uint particleMax;
            //uint particleNotUsed;
            //InactiveBillboardParticleIndex.GetDimensions(particleMax, particleNotUsed);

            //GroupMemoryBarrierWithGroupSync();

            //if (Index > DThreadID.x)
            //{
            //    //InterlockedExchange(ActiveBillboardParticleIndex[DThreadID.x], ActiveBillboardParticleIndex[Index], tempNotUsed);
            //    //SortingData[DThreadID.x].y = ActiveBillboardParticleIndex[DThreadID.x];
            //    //SortingData[DThreadID.x].y = ActiveBillboardParticleIndex[DThreadID.x];

            //}
            //Add particle to Inactive
            uint Index = InactiveParticleIndex.IncrementCounter();
            InactiveParticleIndex[Index] = particleBufferIndex;
        }

        
    }


}