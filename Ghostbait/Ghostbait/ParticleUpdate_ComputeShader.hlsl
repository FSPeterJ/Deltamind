#include "Particle_Definitions.hlsl"
#include "Global.hlsl"


RWStructuredBuffer<BillboardParticle> BillboardParticleBuffer : register(u0);

RWStructuredBuffer<uint> ActiveBillboardParticleIndex : register(u1);
RWStructuredBuffer<uint> InactiveBillboardParticleIndex : register(u2);
RWBuffer<uint> DrawArgs : register(u3);

//groupshared uint ActiveBillboardParticleCount;
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
        uint particleBufferIndex = ActiveBillboardParticleIndex[DThreadID.x];
        BillboardParticle Bparticle = BillboardParticleBuffer[particleBufferIndex];

        //This part may run into issues - we are changing ActiveBillboardParticleIndex while batch iterating it by SV_DispatchThreadID
        // I am unsure if the next batch will have an accurate reference to ActiveBillboardParticleCount but in theory it should with just an atomic operation.
        //if (Bparticle.age > 0.0f)
        {
            Bparticle.age -= FrameTime;

            //I am also unsure if checking again before performing a swap and pop is the most efficent system
            // In theory if the entire thread group has to wait anyways, the first check is irrelevant
            if (Bparticle.age > 0.0f)
            {
                float scaledLife = 1.0 - saturate(Bparticle.age / Bparticle.lifespan);
                Bparticle.color = lerp(Bparticle.startColor, Bparticle.endColor, scaledLife);
                Bparticle.size = lerp(Bparticle.startSize, Bparticle.endSize, scaledLife);

                //Possible Additions: variable rotation speed, acceleration
                //Bparticle.m_Rotation += 0.24 * FrameTime;
                float3 NewPosition = Bparticle.position;
                NewPosition += Bparticle.velocity * FrameTime;
                Bparticle.position = NewPosition;



                BillboardParticleBuffer[particleBufferIndex] = Bparticle;

            }
            else
            {
                //InterlockedAdd(DrawArgs[1], -1);// Wait, can't we use copy subresource count to this buffer?
                //Bparticle.age = -1;
                //Swap
                //BillboardParticleBuffer[particleIndex] = Bparticle; // There is no need to spend time writing data of a dead particle
                uint Index = ActiveBillboardParticleIndex.DecrementCounter();
                uint temp;
                uint particleMax;
                uint particleNotUsed;
                InactiveBillboardParticleIndex.GetDimensions(particleMax, particleNotUsed);
                if (Index + 1 < particleMax)
                {
                    InterlockedExchange(ActiveBillboardParticleIndex[DThreadID.x], ActiveBillboardParticleIndex[Index], temp);

                }
                //Pop - we simply need to move the wall back one and forget about the value left behind
                //InterlockedAdd(ActiveBillboardParticleCount, 1); //This seems unnecessary but if there are problems look here
                //Add particle to Inactive
                Index = InactiveBillboardParticleIndex.IncrementCounter();
                InactiveBillboardParticleIndex[Index - 1] = particleBufferIndex;
            }

        }
    }


}