//Note: This file is configured to act as header file, but named as hlsl for extension support.


cbuffer PerFrameConstantBuffer : register(b0)
{
    float ElapsedTime;
    uint padding[3];
};

SamplerState SamplerWrapLinear : register(s0);
