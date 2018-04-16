Texture2D diffuse : register(t0);
Texture2D emissive : register(t1);
Texture2D normal : register(t2);
Texture2D specular : register(t3);
Texture2D depth : register(t4);
Texture2D unlit : register(t5);

SamplerState sample : register(s0);

struct genericLight
{
    float4 color;
    float3 dir;
    float radius;
    float3 pos;
    float outerRadius;
};

cbuffer lightInfoBuffer : register(b0)
{
    float3 ambientColor;
    float ambientIntensity;
    float3 cameraPos;
};

cbuffer viewProjBuffer : register(b2)
{
    matrix view;
    matrix projection;
};

cbuffer lightBuffer : register(b4)
{
    genericLight lights[83];
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float2x4 calcLight(int i, float3 worldPos, float3 norm)
{
    float2x4 ret = float2x4(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    genericLight theLight = lights[i];
    if (theLight.radius > 0.0f)
    {
        if (theLight.outerRadius > 0.0f)
        {
            float3 dir = normalize(theLight.pos - worldPos);
            float3 coneDir = normalize(theLight.dir);
            float surfaceRatio = saturate(dot(-dir, coneDir));
            float spotFactor = (surfaceRatio > theLight.outerRadius) ? 1.0f : 0.0f;
            float lightRatio = saturate(dot(dir, norm) * spotFactor);
            float atten = 1.0f - saturate((theLight.radius - surfaceRatio) / (theLight.radius - theLight.outerRadius));
            atten *= atten;
            ret._11_12_13_14 = (theLight.color * lightRatio) * atten;
            ret._21 = atten;
        }
        else
        {
            float3 dir = theLight.pos - worldPos;
            if (length(dir) < theLight.radius)
            {
                dir = normalize(dir);
                float lightRatio = saturate(dot(dir, norm));
                float atten = 1.0f - saturate(length(theLight.pos - worldPos) / theLight.radius);
                atten *= atten;
                ret._11_12_13_14 = (theLight.color * lightRatio) * atten;
                ret._21 = atten;
            }
        }
    }
    else
    {
        float3 dir = normalize(theLight.dir);
        dir = -dir;
        float lightRatio = saturate(dot(dir, norm));
        ret._11_12_13_14 = theLight.color * lightRatio;
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

float4 main(PixelShaderInput input) : SV_TARGET
{
    if (unlit.Sample(sample, input.uv).x == 1.0f)
    {
        return saturate(diffuse.Sample(sample, input.uv) + emissive.Sample(sample, input.uv));
    }
    float4 finalLight = float4(ambientColor * ambientIntensity, 1.0f);
    float4 finalSpec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 norm = (normal.Sample(sample, input.uv) - 0.5f).xyz * 2.0f;
    float specIntense = specular.Sample(sample, input.uv).x;
    float4 tempDepth = float4(float2(input.uv.x * 2.0f - 1.0f, (1.0f - input.uv.y) * 2.0f - 1.0f), depth.Sample(sample, input.uv).xy);
    float4 posAlmost = mul(projection, float4(float3(tempDepth.x * tempDepth.w, tempDepth.y * tempDepth.w, tempDepth.z * tempDepth.w), tempDepth.w));
    posAlmost.w = 1.0f;
    posAlmost = mul(posAlmost, view);
    for (int i = 0; i < 83; ++i)
    {
        if (lights[i].color.w == 0.0f)
            break;
        float2x4 result = calcLight(i, posAlmost.xyz, norm);
        finalLight += result._11_12_13_14;
        finalSpec += calcSpec(i, result._21, posAlmost.xyz, norm, specIntense);
    }
    float4 diffuseColor = diffuse.Sample(sample, input.uv);
    float4 emissiveColor = emissive.Sample(sample, input.uv);
    return float4(saturate((float4(finalLight.xyz, 1.0f) * diffuseColor) + emissiveColor + finalSpec).xyz, 1.0f);
}
