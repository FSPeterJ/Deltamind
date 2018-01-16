texture2D diffuse : register(t0);
texture2D specular : register(t1);
texture2D emissive : register(t2);

SamplerState sample : register(s0);

cbuffer dirLightBuffer : register(b0)
{
    float4 lightColor;
    float3 lightDir;
    float ambient;
};

cbuffer factorBuffer : register(b1)
{
    float diffuseFactor;
    float specularFactor;
    float emissiveFactor;
    float morepadding;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 norm : TEXCOORD1;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 dir = normalize(lightDir);
    dir = -dir;
    float lightRatio = saturate(saturate(dot(dir, input.norm)));
    //atten *= atten;
    float4 finalLight = lightColor * saturate(lightRatio + ambient);
    //finalLight *= atten;
    float4 diffuseColor = diffuse.Sample(sample, input.uv);// * diffuseFactor;
    float4 emissiveColor = emissive.Sample(sample, input.uv) * emissiveFactor;

    //float3 reflectionDir = reflect(lightDir, input.norm);
    //float3 dirToCam = normalize(cameraPos - lightPos);
    //float specScale = 0.3f * pow(saturate(dot(reflectionDir, dirToCam)), 1.0f);
    //float specIntense = specular.Sample(sample, input.uv).x;
    //float4 specColor = specIntense * specScale * lightColor;
    //return (finalLight * diffuseColor) + emissiveColor;// + specColor;
    return diffuseColor;
}