//Note: This file is configured to act as header file, but named as hlsl for extension support.


//To optimize this further, split the calculation data from the render data into seperate buffers
// It is important that 16 byte alignment is maintained.
//https://developer.nvidia.com/content/understanding-structured-buffer-performance
struct BillboardParticle // (48 bytes) 500,000  = 25MB of VRAM for all particle data (not including textures)
{
     //Dynamic data  (16 bytes)
    float3 position;
    float rotation;
    
    //Dynamic data  (16 bytes)
    float3 velocity;
    float age;

    //Constant data (16 bytes)
    float lifespan; // how long the particle will last
    float startSize; //particle scale over time data
    float endSize;
    uint texturedata; // 12 bits - U Axis UV end | 12 bits - V Axis UV end | 8 bits - texture W index
    //This limits a particle texture max size to 4095x4095 (dx11 max size is 16384x16384) with max indexing of 256 different textures (one 4095 is 64MB of VRAM unless DXT1 compression is used)
    //The data stored is the endpoint for the UV within the texturearray as the texture may be smaller than the actual sample array size

};

cbuffer InactiveParticles : register(b10)
{
    uint InactiveParticleCount;
    uint IPCpad[3];
};

// The number of alive particles this frame
cbuffer ActiveParticles : register(b11)
{
    uint ActiveParticleCount;
    uint APCpad[3];
};