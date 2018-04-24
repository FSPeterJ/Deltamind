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
    float EmissionCount;
    float Mass;
    uint PerEmission;

    float3 acceleration;
    float velocityLossFactor;

    float3 Gravity;
    float Unused;
};




[numthreads(1024, 1, 1)]
void main(uint3 DThreadID : SV_DispatchThreadID)
{

    //0.001
    //0.002
    uint threadBatch = (DThreadID.x / PerEmission) + 1;

    float emTimestamp = emissionRateMS * threadBatch;
    float totalTime = FrameTime + emissionOverflow;
    //Stop other threads from attempting to process particle emissions if there are none left to process
    if (DThreadID.x < InactiveParticleCount && emTimestamp < totalTime )
    {
        Particle particle = (Particle) 0;
        float3 randomPosition;

        particle.mass = Mass;

        float2 uv = float2(DThreadID.x / 1024.0f, ElapsedTime);
        float randomAnglesx = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).x;
        float randomAnglesy = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).y;


                static const float PI = 3.14159265f;

       
        float z = randomAnglesx * 2 + randomAnglesx;
        float phi = randomAnglesy *2 * PI;
        float x = sqrt(1-z*z ) * cos(phi);
        float y = sqrt(1-z*z ) * sin(phi);
        float3 randomDirection = float3(x, y, z);
        

        //particle.velocity = float3(0, 0, 0);
        //particle.position = randomDirection * VelocityMagnatude + float3(0, 1, 1);

 
        particle.velocity = VelocityMagnatude * randomDirection;
        particle.position = Position + particle.velocity * (emissionRateMS * threadBatch + emissionOverflow);
        particle.age = ParticleLifeSpan - emissionRateMS * threadBatch + emissionOverflow;


        particle.lifespan = ParticleLifeSpan;
        particle.startSize = StartSize;
        particle.endSize = EndSize;
        particle.texturedata = TextureIndex;

        particle.startColor = startColor;
        particle.endColor = endColor;


        particle.Gravity = Gravity;
        particle.velocityLossFactor = velocityLossFactor;




        
        uint index = InactiveParticleIndex.DecrementCounter(); //Decrement returns POST-decrement value 
        uint particleindex = InactiveParticleIndex[index];

        ParticleBuffer[particleindex] = particle;
        index = ActiveParticleIndex.IncrementCounter(); //Increment returns PRE-increment value (what?)
        ActiveParticleIndex[index] = particleindex;
    }
}