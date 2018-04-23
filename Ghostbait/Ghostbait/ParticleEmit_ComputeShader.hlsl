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
    float paddingEM[2];
};




[numthreads(1024, 1, 1)]
void main(uint3 DThreadID : SV_DispatchThreadID)
{

    //0.001
    //0.002
    float emTimestamp = emissionRateMS * (DThreadID.x + 1);
    float totalTime = FrameTime + emissionOverflow;
    //Stop other threads from attempting to process particle emissions if there are none left to process
    if (DThreadID.x < InactiveParticleCount && emTimestamp < totalTime)
    {
        Particle particle = (Particle) 0;
        float3 randomPosition;

        //float randomAnglesx = (RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).x % xAngleVariance * 2) -xAngleVariance;
        //float randomAnglesy = (RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).y % yAngleVariance * 2) -yAngleVariance;
        //float randomAnglesy = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).y;
        
        //float x = sin() * (cos(randomAnglesx) + sin(randomAnglesy)) + cos(xAngleVariance * 2);


        float2 uv = float2(DThreadID.x / 1024.0f, ElapsedTime);
        float randomAnglesx = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).x;
        float randomAnglesy = RandomNumbers.SampleLevel(SamplerWrapLinear, uv, 0).y;
        //float2 Angles = float2((randomAnglesx % xAngleVariance * 2) - xAngleVariance, (randomAnglesy % yAngleVariance * 2) - yAngleVariance);

        //float2
        //float2 Angles = float2(sqrt(xAngleVariance) * cos(randomAnglesx), sqrt(yAngleVariance) * sin(randomAnglesy));
        //float2 Angles = float2(sqrt(randomAnglesx) * cos(xAngleVariance * 2), sqrt(randomAnglesy) * sin(yAngleVariance * 2));
        //float2 Angles = float2(sqrt(randomAnglesx) * cos(xAngleVariance), sqrt(randomAnglesy) * sin(yAngleVariance));
        //float2 Angles = float2(sin(* cos(xAngleVariance), sqrt(randomAnglesy) * sin(yAngleVariance));

                static const float PI = 3.14159265f;

        //float z = randomAnglesx * (1 - cos(xAngleVariance)) + cos(xAngleVariance);        
        //float z = randomAnglesx*2;         
        float z = randomAnglesx * 2 + randomAnglesx;
        //float z = -(randomAnglesx * PI);
        float phi = randomAnglesy *2 * PI;
        //float phi = (randomAnglesy + randomAnglesy) * PI;
        float x = sqrt(1-z*z ) * cos(phi);
        float y = sqrt(1-z*z ) * sin(phi);
        float3 randomDirection = float3(x, y, z);
        

        //float x = randomAnglesx * (1 - cos(randomAnglesx)) + cos(randomAnglesx);

        //    //float r = sqrt(max(1E-30, x * x - 1)); 
        //float r = sqrt(x * x); 
        //float random_angle = 2 * PI * randomAnglesx;
        //float y = r * sin(random_angle);
        //float z = r * cos(random_angle);


        //float3 randomDirection = float3(cos(Angles.y) * sin(Angles.x), sin(Angles.y), cos(Angles.y) * cos(Angles.x));


        float3 V = Velocity;
        float velocityMagnitude = length(V);
        particle.velocity = float3(0, 0, 0);
        //particle.velocity = VelocityMagnatude * randomDirection;
        //particle.position = Position + float3(0, 1, 1) + particle.velocity * (emissionRateMS * DThreadID.x + emissionOverflow);
        particle.position = randomDirection * VelocityMagnatude + float3(0, 1, 1);
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