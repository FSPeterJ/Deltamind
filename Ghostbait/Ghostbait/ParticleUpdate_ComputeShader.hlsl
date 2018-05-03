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

[numthreads(512, 1, 1)]
void main(  uint3 DThreadID : SV_DispatchThreadID,
		    uint GroupIndex : SV_GroupIndex)
{
    //The reason behind this system is to quit out of cycling particles early.  From what I understand that way that GPU thread groups work
    //is they are processed in batches.  This means if 64 threads are assigned a task and only 2 of them are active, the entire thread group must wait 
    //until all have processed in order to release the batch.

    if (GroupIndex < ActiveParticleCount)
    {
        uint particleBufferIndex = ActiveParticleIndex[GroupIndex];
        Particle particle = ParticleBuffer[particleBufferIndex];
        const float3 Gravity = float3(0.0, -9.81, 0.0);

        particle.age = particle.age - FrameTime;

        if (particle.age > 0.0f)
        {






            float scaledLife = saturate(1.0 - saturate(particle.age / particle.lifespan));
            particle.color = lerp(particle.startColor, particle.endColor, scaledLife);
            particle.size = lerp(particle.startSize, particle.endSize, scaledLife);
            //particle.size = 0.1f + scaledLife * 0.1f;
            ////Possible variable rotation speed, acceleration
            particle.rotation += 0.24 * FrameTime;
            particle.velocity += particle.Gravity * FrameTime * particle.mass;

            particle.position = particle.position + particle.velocity * FrameTime + particle.acceleration * FrameTime;
            particle.distanceToCamera = length(particle.position - CameraCenterpoint.xyz);
            //
            //particle.position = float3(GroupIndex * 0.5f, 1.5f - GroupIndex * 0.1f, 3);
            //float indexcolorer = ((float) particleBufferIndex / (float) MaxParticles);

            //particle.color = float4(1.0f * indexcolorer, 0, 1.0f * (1.0f - indexcolorer), 1.0f);



            ParticleBuffer[particleBufferIndex] = particle;



            SortingData.Append(float2(particle.distanceToCamera, asfloat(particleBufferIndex)));
        }
        else
        {

            ActiveParticleIndex.DecrementCounter();
            uint Index = InactiveParticleIndex.IncrementCounter();
            DeviceMemoryBarrier();
            InactiveParticleIndex[Index] = particleBufferIndex;
        }

        
    }


}