//Note: This file is configured to act as header file, but named as hlsl for extension support.


cbuffer PerFrameConstantBuffer : register(b0)
{
    float FrameTime;
    float ElapsedTime;
    float lastElapsed;
    float paddingThing;

    float3 CameraCenterpoint;
    float paddingThing2;
};

SamplerState SamplerWrapLinear : register(s0);
