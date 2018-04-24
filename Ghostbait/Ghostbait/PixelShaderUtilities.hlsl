
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


float2x4 calcLight(int i, float3 worldPos, float3 norm)
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
            float lightRatio = saturate(dot(dir, norm) * spotFactor);
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
                float lightRatio = saturate(dot(dir, norm));
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
        float lightRatio = saturate(dot(dir, norm));
        ret._11_12_13_14 = lights[i].color * lightRatio;
    }
    return ret;
}

float4 calcSpec(int i, float atten, float3 worldPos, float3 norm, float specIntense)
{
    float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //I would prefer to not have to do a lot of these calculations but hlsl doesn't do pointers or globals
    
    if (lights[i].radius > 0.0f && specIntense != 0.0f)
    {
        if (lights[i].outerRadius > 0.0f)
        {
            float3 dir = normalize(lights[i].pos - worldPos);

            float3 reflectionDir = reflect(-dir, norm);
            float3 dirToCam = normalize(cameraPos - worldPos);
            float specScale = 1.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
            ret = specIntense * specScale * lights[i].color * atten;
        }
        else
        {
            float3 dir = normalize(lights[i].pos - worldPos);
            float3 reflectionDir = reflect(-dir, norm);
            float3 dirToCam = normalize(cameraPos - worldPos);
            float specScale = 1.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
            ret = specIntense * specScale * lights[i].color * atten;
        }
    }
    else if (specIntense != 0.0f)
    {
        float3 dir = normalize(lights[i].dir);
        float3 reflectionDir = reflect(dir, norm);
        float3 dirToCam = normalize(cameraPos - worldPos);
        float specScale = 1.0f * pow(saturate(dot(reflectionDir, dirToCam)), 25.0f);
        ret = specIntense * specScale * lights[i].color;
    }
    return ret;
}