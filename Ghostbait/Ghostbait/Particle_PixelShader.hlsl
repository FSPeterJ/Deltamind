//#include "PixelShaderUtilities.hlsl"


cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer factorBuffer : register(b2)
{
    float diffuseFactor;
    float specularFactor;
    float emissiveFactor;
    float morepadding;
};

Texture2D emissive : register(t1);
Texture2D normal : register(t2);
Texture2D specular : register(t3);
Texture2D depth : register(t4);
Texture2D unlit : register(t5);
Texture2DArray diffusearray : register(t6);

SamplerState sample : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 uvw : TEXCOORD0;
    float4 color : COLOR0;
};

struct PixelShaderOutput
{
    float4 diffuse : SV_TARGET0;
    float4 emissive : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 specular : SV_TARGET3;
    float4 depth : SV_TARGET4;
    float4 unlit : SV_TARGET5;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    float4 diffuseFloat = diffusearray.Sample(sample,input.uvw);
    //float4 diffuseFloat = diffusearray.Sample(sample, (float2)input.uvw);
    //if (diffuseFloat.w == 0.0f)
    //    discard;
    //output.diffuse = float4(diffuseFloat.xyz * diffuseFactor, diffuseFloat.w);    
    //output.diffuse = float4(diffuseFloat.xyz * 1.0f, diffuseFloat.w);
    //output.diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);
    output.diffuse = input.color;
    output.emissive = input.color;
    //output.diffuse = diffuseFloat;
    //output.emissive = diffuseFloat;
    output.normal = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.depth = float4(input.pos.z, input.pos.z, input.pos.z, 1.0f);
    output.unlit = float4(1.0f, 1.0f,1.0f, 1.0f);
    return output;
}


