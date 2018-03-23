#include "Particle_Definitions.hlsl"
#include "Global.hlsl"


Texture2D RandomNumbers : register(t0);

//This setup works very similar to how our pool system works.
// There is a pool of particles and then an inactive / active list.  Instead of pointers we use indicies.  


AppendStructuredBuffer<uint> ActiveBillboardParticleIndex : register(u1); //We are simply using the Append view;
ConsumeStructuredBuffer<uint> InactiveBillboardParticleIndex : register(u2);

cbuffer EmitterConstantBuffer : register(b1)
{
    float3 Position;
    uint MaxParticlesThisFrame;

    float3 Velocity;
    float VelocityMagnatude;

    float3 ParticlePositionVariance;
    float ParticleVelocityVariance;
	
    float StartSize;
    float ParticleLifeSpan;
    float EndSize;
    uint TextureIndex;
    uint pads[1];
};




[numthreads(1024, 1, 1)]
void main(uint3 DThreadID : SV_DispatchThreadID)
{
    //Stop other threads from attempting to process particle emissions if there are none left to process
    if (DThreadID.x < InactiveParticleCount && DThreadID.x < MaxParticlesThisFrame)
    {
        BillboardParticle particle = (BillboardParticle) 0;
        float3 randomPosition;

        float2 uv = float2(DThreadID.x / 1024.0, ElapsedTime);
        float3 randomvelocity = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).xyz;

        particle.age = ParticleLifeSpan;
        particle.compactedData = TextureIndex;
        particle.lifespan = ParticleLifeSpan;
        particle.velocity = Velocity + (randomvelocity * VelocityMagnatude * ParticleVelocityVariance);

        uint index = InactiveBillboardParticleIndex.Consume();
        //uint inactiveIndex = InactiveBillboardParticleIndex.DecrementCounter();
        //InterlockedExchange(InactiveBillboardParticleCount, , index);
        //InactiveBillboardParticleIndex.
        BillboardParticleBuffer[index] = particle;
        ActiveBillboardParticleIndex.Append(index);
    }
}