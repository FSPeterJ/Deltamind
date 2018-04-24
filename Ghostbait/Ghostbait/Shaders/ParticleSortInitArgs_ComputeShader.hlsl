#include "../Particle_Definitions.hlsl"

RWBuffer<uint> DispatchParameters : register(u5);

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    DispatchParameters[0] = ((ActiveParticleCount) >> 9)+1;
    DispatchParameters[1] = 1;
    DispatchParameters[2] = 1;
    DispatchParameters[3] = 0;
}