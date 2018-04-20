#include "Particle_Definitions.hlsl"
#include "Global.hlsl"


Texture2D RandomNumbers : register(t0);

//This setup works very similar to how our pool system works.
// There is a pool of particles and then an inactive / active list.  Instead of pointers we use indicies.  

RWStructuredBuffer<Particle> ParticleBuffer : register(u0);

RWStructuredBuffer<uint> ActiveParticleIndex : register(u1); 
RWStructuredBuffer<uint> InactiveParticleIndex : register(u2);
RWBuffer<uint> DrawArgs : register(u3);



//float3 float float3 float float3 float float float float uint float4 float4 float uint float float float float3
cbuffer EmitterConstantBuffer : register(b3)
{
    float3 Position;
    float emissionRateMS;

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

    float emissionOverflow;
    float paddingEM[3];
};




[numthreads(1024, 1, 1)]
void main(uint3 DThreadID : SV_DispatchThreadID)
{

    //0.001
    //0.002
    float emTimestamp = emissionRateMS * (DThreadID.x + 1) ;
    float totalTime = FrameTime + emissionOverflow;
    //Stop other threads from attempting to process particle emissions if there are none left to process
    if (DThreadID.x < InactiveParticleCount && emTimestamp < totalTime)
    {
        Particle particle = (Particle) 0;
        float3 randomPosition;

        //float2 uv = float2(DThreadID.x / 1024.0, ElapsedTime);
        //float3 randomvelocity = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).xyz;

        particle.velocity = Velocity; //+ //(randomvelocity * VelocityMagnatude * ParticleVelocityVariance);
        particle.position = Position + float3(0, 1, 1) + particle.velocity * (emissionRateMS * DThreadID.x + emissionOverflow);
        particle.age = ParticleLifeSpan;


        particle.lifespan = ParticleLifeSpan;
        particle.startSize = StartSize;
        particle.endSize = EndSize;
        particle.texturedata = TextureIndex;

        particle.startColor = startColor;
        particle.endColor = endColor;


        
        uint index = InactiveParticleIndex.DecrementCounter(); // The conversion from count to 
        uint particleindex = InactiveParticleIndex[index];

        ParticleBuffer[particleindex] = particle;
        index = ActiveParticleIndex.IncrementCounter();
        ActiveParticleIndex[index] = particleindex;
    }
}