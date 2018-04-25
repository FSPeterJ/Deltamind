//Note: This file is configured to act as a header file, but named as hlsl for extension support.


//To optimize this further, split the calculation data from the render data into seperate buffers
// It is important that 16 byte alignment is maintained.
//https://developer.nvidia.com/content/understanding-structured-buffer-performance
struct Particle 
{
             //Dynamic data  (16 bytes)
    float3  position;
    float   rotation;
    
            //Dynamic data  (16 bytes)
    float3  velocity;
    float   age;

            //Constant data (16 bytes)
    float   lifespan; // how long the particle will last
    float   startSize; //particle scale over time data
    float   endSize;
    uint    texturedata; // 12 bits - U Axis UV end | 12 bits - V Axis UV end | 8 bits - texture W index
            //This limits a particle texture max size to 4095x4095 (dx11 max size is 16384x16384) with max indexing of 256 different textures (one 4095 is 64MB of VRAM unless DXT1 compression is used)
            //The data stored is the endpoint for the UV within the texturearray as the texture may be smaller than the actual sample array size

            //Constant data (16 bytes)
    float   size;
    float   distanceToCamera;
    float   mass;
    float   size3;

            //Constant data (16 bytes)
    float4  color;
            //Constant data (16 bytes)
    float4  startColor;
            //Constant data (16 bytes)
    float4  endColor;

            //Constant data (16 bytes)
    float3  acceleration;
    float   velocityLossFactor;

            //Constant data (16 bytes)
    float3  Gravity;
    float   Unused;
};

// The number of alive particles this frame

//In reality, it would be best to just use 1 cbuff
cbuffer ActiveParticles : register(b1)
{
    uint ActiveParticleCount;
    uint MaxParticles;
    uint APCpad[2];
};

cbuffer InactiveParticles : register(b2)
{
    uint InactiveParticleCount;
    uint IPCpad[3];
};

