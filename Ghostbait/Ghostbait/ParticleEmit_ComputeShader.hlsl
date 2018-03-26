#include "Particle_Definitions.hlsl"
#include "Global.hlsl"


Texture2D RandomNumbers : register(t0);

//This setup works very similar to how our pool system works.
// There is a pool of particles and then an inactive / active list.  Instead of pointers we use indicies.  


RWStructuredBuffer<uint> ActiveBillboardParticleIndex : register(u11); 
RWStructuredBuffer<uint> InactiveBillboardParticleIndex : register(u12);
RWBuffer<uint> DrawArgs : register(u13);


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
        particle.texturedata = TextureIndex;
        particle.lifespan = ParticleLifeSpan;
        particle.velocity = Velocity + (randomvelocity * VelocityMagnatude * ParticleVelocityVariance);

        uint index = InactiveBillboardParticleIndex.DecrementCounter();
        uint particleindex = InactiveBillboardParticleIndex[index];
        //uint inactiveIndex = InactiveBillboardParticleIndex.DecrementCounter();
        //InterlockedExchange(InactiveBillboardParticleCount, , index);
        BillboardParticleBuffer[particleindex] = particle;
        index = ActiveBillboardParticleIndex.IncrementCounter();
        ActiveBillboardParticleIndex[index] = particleindex;

    }
}