
#include "../Particle_Definitions.hlsl"

//#include "../Global.hlsl"
//--------------------------------------------------------------------------------------
// Structured Buffers
//--------------------------------------------------------------------------------------
RWStructuredBuffer<uint> ActiveParticleIndex : register(u1);
RWStructuredBuffer<float2> SortingData : register(u4);

//--------------------------------------------------------------------------------------
// Bitonic Sort Compute Shader
//--------------------------------------------------------------------------------------

cbuffer SortConstants : register(b4)
{
    int4 job_params;
};

[numthreads(256, 1, 1)]
void main(uint3 Gid : SV_GroupID,
						uint3 GTid : SV_GroupThreadID)
{
    int4 tgp;

    tgp.x = Gid.x * 256;
    tgp.y = 0;
    tgp.z = ActiveParticleCount;
    tgp.w = min(512, max(0, ActiveParticleCount - Gid.x * 512));

    uint localID = tgp.x + GTid.x; // calculate threadID within this sortable-array

    uint index_low = localID & (job_params.x - 1);
    uint index_high = 2 * (localID - index_low);

    uint index = tgp.y + index_high + index_low;
    uint nSwapElem =  index_high + job_params.y + job_params.z * index_low;

    if (nSwapElem < tgp.z)
    {
        float2 a = SortingData[index];
        float2 b = SortingData[nSwapElem];

        if (a.x > b.x)
        {
            SortingData[index] = b;
            SortingData[nSwapElem] = a;
            
            ActiveParticleIndex[index] = asuint(b.y);
            ActiveParticleIndex[nSwapElem] = asuint(a.y);
        }
    }
}
