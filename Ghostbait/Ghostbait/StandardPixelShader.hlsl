Texture2D diffuse : register(t0);
Texture2D specular : register(t1);
Texture2D emissive : register(t2);

SamplerState sample : register(s0);

struct genericLight
{
    float4 color;
    float3 dir;
    float radius;
    float3 pos;
    float outerRadius;
};

cbuffer lightBuffer : register(b0)
{
    genericLight lights[83];
    float3 ambientColor;
    float ambientIntensity;
    float3 cameraPos;
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
    float3 worldPos : TEXCOORD2;
};

float4 calcLight(int i, PixelShaderInput input)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (lights[i].radius > 0.0f)
    {
        if (lights[i].outerRadius > 0.0f)
        {
            float3 dir = normalize(lights[i].pos - input.worldPos);
            float3 coneDir = normalize(lights[i].dir);
            float surfaceRatio = saturate(dot(-dir, coneDir));
            float spotFactor = (surfaceRatio > lights[i].outerRadius) ? 1.0f : 0.0f;
            float lightRatio = saturate(dot(dir, input.norm) * spotFactor);
            float atten = 1.0f - saturate((lights[i].radius - surfaceRatio) / (lights[i].radius - lights[i].outerRadius));
            atten *= atten;
            ret = (lights[i].color * lightRatio) * atten;
        }
        else
        {
            float3 dir = lights[i].pos - input.worldPos;
            if (length(dir) < lights[i].radius)
            {
                dir = normalize(dir);
                float lightRatio = saturate(dot(dir, input.norm));
                float atten = 1.0f - saturate(length(lights[i].pos - input.worldPos) / lights[i].radius);
                atten *= atten;
                ret = (lights[i].color * lightRatio) * atten;
            }
        }
    }
    else
    {
        float3 dir = normalize(lights[i].dir);
        dir = -dir;
        float lightRatio = saturate(dot(dir, input.norm));
        ret = lights[i].color * lightRatio;
    }
    return ret;
}

float4 calcSpec(int i, PixelShaderInput input)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //I would prefer to not have to do a lot of these calculations but hlsl doesn't do pointers or globals
    
    if (lights[i].radius > 0.0f && specularFactor != 0.0f)
    {
        if (lights[i].outerRadius > 0.0f)
        {
            float3 dir = normalize(lights[i].pos - input.worldPos);

            float3 reflectionDir = reflect(-dir, input.norm);
            float3 dirToCam = normalize(cameraPos - input.worldPos);
            float specScale = 3.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
            float specIntense = specular.Sample(sample, input.uv).x * specularFactor;
            ret = specIntense * specScale * lights[i].color;
        }
        else
        {
            float3 dir = normalize(lights[i].pos - input.worldPos);
            float3 reflectionDir = reflect(-dir, input.norm);
            float3 dirToCam = normalize(cameraPos - input.worldPos);
            float specScale = 3.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
            float specIntense = specular.Sample(sample, input.uv).x * specularFactor;
            ret = specIntense * specScale * lights[i].color;
        }
    }
    else
    {
        float3 dir = normalize(lights[i].dir);
        float3 reflectionDir = reflect(dir, input.norm);
        float3 dirToCam = normalize(cameraPos - input.worldPos);
        float specScale = 3.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
        float specIntense = specular.Sample(sample, input.uv).x * specularFactor;
        ret = specIntense * specScale * lights[i].color;
    }
    return ret;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 finalLight = float4(ambientColor * ambientIntensity, 1.0f);
    float4 finalSpec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    [unroll(83)] for (int i = 0; i < 83; ++i)
    {
        if (lights[i].color.w == 0.0f)
            break;
        finalLight += calcLight(i, input);
        finalSpec += calcSpec(i, input);
    }
    float4 diffuseColor = diffuse.Sample(sample, input.uv) * diffuseFactor;
    float4 emissiveColor = emissive.Sample(sample, input.uv) * emissiveFactor;
    
    return saturate(((finalLight + finalSpec) * diffuseColor) + emissiveColor);
}
