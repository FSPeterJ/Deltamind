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
    //The reason behind this system is to quit out of cycling particles early.  From what I understand that way that GPU thread groups work
    //is they are processed in batches.  This means if 64 threads are assigned a task and only 2 of them are active, the entire thread group must wait 
    //until all have processed in order to release the batch.

    if (DThreadID.x < ActiveParticleCount)
    {
        uint particleBufferIndex = ActiveParticleIndex[DThreadID.x];
        Particle particle = ParticleBuffer[particleBufferIndex];
        const float3 Gravity = float3(0.0, -9.81, 0.0);


        particle.age -= FrameTime;

        if (particle.age > 0.0f)
        {
            float scaledLife = 1.0 - saturate(particle.age / particle.lifespan);
            particle.color = lerp(particle.startColor, particle.endColor, scaledLife);
            particle.size = lerp(particle.startSize, particle.endSize, scaledLife);

            //Possible variable rotation speed, acceleration
            particle.rotation += 0.24 * FrameTime;
            float3 NewPosition = particle.position;
            particle.velocity += particle.Gravity * FrameTime * particle.mass;

            NewPosition += particle.velocity * FrameTime;
            particle.position = NewPosition;
            particle.distanceToCamera = length(NewPosition - CameraCenterpoint.xyz);


            ParticleBuffer[particleBufferIndex] = particle;
            SortingData.Append(float2(particle.distanceToCamera, asfloat(particleBufferIndex)));
        }
        else
        {

            ActiveParticleIndex.DecrementCounter();
            uint Index = InactiveParticleIndex.IncrementCounter();
            InactiveParticleIndex[Index] = particleBufferIndex;
        }

        
    }


}