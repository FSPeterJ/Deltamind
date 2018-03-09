Texture2D diffuse : register(t0);
Texture2D emissive : register(t1);
Texture2D normal : register(t2);
Texture2D specular : register(t3);
Texture2D depth : register(t4);

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

cbuffer viewProjBuffer : register(b2)
{
    matrix view;
    matrix projection;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 norm : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
};

float2x4 calcLight(int i, PixelShaderInput input, float3 worldPos)
{
    float2x4 ret = float2x4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    if (lights[i].radius > 0.0f)
    {
        if (lights[i].outerRadius > 0.0f)
        {
            float3 dir = normalize(lights[i].pos - worldPos);
            float3 coneDir = normalize(lights[i].dir);
            float surfaceRatio = saturate(dot(-dir, coneDir));
            float spotFactor = (surfaceRatio > lights[i].outerRadius) ? 1.0f : 0.0f;
            float lightRatio = saturate(dot(dir, input.norm) * spotFactor);
            float atten = 1.0f - saturate((lights[i].radius - surfaceRatio) / (lights[i].radius - lights[i].outerRadius));
            atten *= atten;
            ret._11_12_13_14 = (lights[i].color * lightRatio) * atten;
            ret._21 = atten;
        }
        else
        {
            float3 dir = lights[i].pos - worldPos;
            if (length(dir) < lights[i].radius)
            {
                dir = normalize(dir);
                float lightRatio = saturate(dot(dir, input.norm));
                float atten = 1.0f - saturate(length(lights[i].pos - worldPos) / lights[i].radius);
                atten *= atten;
                ret._11_12_13_14 = (lights[i].color * lightRatio) * atten;
                ret._21 = atten;
            }
        }
    }
    else
    {
        float3 dir = normalize(lights[i].dir);
        dir = -dir;
        float lightRatio = saturate(dot(dir, input.norm));
        ret._11_12_13_14 = lights[i].color * lightRatio;
    }
    return ret;
}

float4 calcSpec(int i, PixelShaderInput input, float atten, float3 worldPos)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //I would prefer to not have to do a lot of these calculations but hlsl doesn't do pointers or globals
    
    if (lights[i].radius > 0.0f && specularFactor != 0.0f)
    {
        if (lights[i].outerRadius > 0.0f)
        {
            float3 dir = normalize(lights[i].pos - worldPos);

            float3 reflectionDir = reflect(-dir, input.norm);
            float3 dirToCam = normalize(cameraPos - worldPos);
            float specScale = 1.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
            float specIntense = specular.Sample(sample, input.uv).x * specularFactor;
            ret = specIntense * specScale * lights[i].color * atten;
        }
        else
        {
            float3 dir = normalize(lights[i].pos - worldPos);
            float3 reflectionDir = reflect(-dir, input.norm);
            float3 dirToCam = normalize(cameraPos - worldPos);
            float specScale = 1.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
            float specIntense = specular.Sample(sample, input.uv).x * specularFactor;
            ret = specIntense * specScale * lights[i].color * atten;
        }
    }
    else
    {
        float3 dir = normalize(lights[i].dir);
        float3 reflectionDir = reflect(dir, input.norm);
        float3 dirToCam = normalize(cameraPos - worldPos);
        float specScale = 1.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
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
        float2x4 result = calcLight(i, input, input.worldPos);
            finalLight += result._11_12_13_14;
            finalSpec += calcSpec(i, input, result._21, input.worldPos);
    }
    float4 diffuseColor = diffuse.Sample(sample, input.uv) * diffuseFactor;
    float4 emissiveColor = emissive.Sample(sample, input.uv) * emissiveFactor;
    return saturate((finalLight * diffuseColor) + emissiveColor + finalSpec);
}
