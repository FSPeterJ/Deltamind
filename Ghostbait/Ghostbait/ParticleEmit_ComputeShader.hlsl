#include "Particle_Definitions.hlsl"
#include "Global.hlsl"


Texture2D RandomNumbers : register(t0);

//This setup works very similar to how our pool system works.
// There is a pool of particles and then an inactive / active list.  Instead of pointers we use indicies.  

RWStructuredBuffer<BillboardParticle> BillboardParticleBuffer : register(u0);

RWStructuredBuffer<uint> ActiveBillboardParticleIndex : register(u1); 
RWStructuredBuffer<uint> InactiveBillboardParticleIndex : register(u2);
RWBuffer<uint> DrawArgs : register(u3);


cbuffer EmitterConstantBuffer : register(b3)
{
    float3 Position;
    uint MaxParticlesThisFrame;

    float3 Velocity;
    float ParticleVelocityVariance;

    float3 ParticlePositionVariance;
    float VelocityMagnatude;
	

    float StartSize;
    float EndSize;
    float ParticleLifeSpan;
    uint TextureIndex;

    float4 startColor;
    float4 endColor;

    float rotationVarience;
    uint properties;
    float xAngleVariance;
    float yAngleVariance;
};




[numthreads(1024, 1, 1)]
void main(uint3 DThreadID : SV_DispatchThreadID)
{
    //Stop other threads from attempting to process particle emissions if there are none left to process
    if (DThreadID.x < InactiveParticleCount && DThreadID.x < MaxParticlesThisFrame)
    {
        BillboardParticle particle = (BillboardParticle) 0;
        float3 randomPosition;

        //float2 uv = float2(DThreadID.x / 1024.0, ElapsedTime);
       // float3 randomvelocity = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).xyz;

        particle.position = float3(0, 1, 1);

        particle.velocity = float3(0, 0, 1); //+ //(randomvelocity * VelocityMagnatude * ParticleVelocityVariance);
        particle.age = ParticleLifeSpan;


        particle.lifespan = ParticleLifeSpan;
        particle.startSize = StartSize;
        particle.endSize = EndSize;
        particle.texturedata = TextureIndex;

        particle.startColor = startColor;
        particle.endColor = endColor;

        uint particleMax;
        uint particleNotUsed;
        InactiveBillboardParticleIndex.GetDimensions(particleMax, particleNotUsed);
        
        uint index = InactiveBillboardParticleIndex.DecrementCounter();
        uint particleindex = InactiveBillboardParticleIndex[index];
        //uint inactiveIndex = InactiveBillboardParticleIndex.DecrementCounter();
        //InterlockedExchange(InactiveBillboardParticleCount, , index);
        BillboardParticleBuffer[particleindex] = particle;
        index = ActiveBillboardParticleIndex.IncrementCounter();
        ActiveBillboardParticleIndex[index] = particleindex;

    }
}