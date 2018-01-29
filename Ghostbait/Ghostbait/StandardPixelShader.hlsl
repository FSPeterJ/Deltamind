texture2D diffuse : register(t0);
texture2D specular : register(t1);
texture2D emissive : register(t2);

SamplerState sample : register(s0);

struct genericLight {
	float4 color;
	float3 dir;
    float radius;
	float3 pos;
	float outerRadius;
};

cbuffer lightBuffer : register(b0) {
	genericLight lights[83];
    float3 ambientColor;
    float ambientIntensity;
};

cbuffer factorBuffer : register(b1) {
	float diffuseFactor;
	float specularFactor;
	float emissiveFactor;
	float morepadding;
};

struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 norm : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 finalLight = float4(ambientColor * ambientIntensity, 1.0f);
    [unroll(83)] for (int i = 0; i < 83; ++i)
    {
        if (lights[i].color.w == 0.0f)
            break;
        if(lights[i].radius > 0.0f)
        {
            if(lights[i].outerRadius > 0.0f)
            {
                float3 dir = normalize(lights[i].pos - input.worldPos);
                float3 coneDir = normalize(lights[i].dir);
                float surfaceRatio = saturate(dot(-dir, coneDir));
                float spotFactor = (surfaceRatio > lights[i].outerRadius) ? 1.0f : 0.0f;
                float lightRatio = saturate(dot(dir, input.norm) * spotFactor);
                float atten = 1.0f - saturate((lights[i].radius - surfaceRatio) / (lights[i].radius - lights[i].outerRadius));
                atten *= atten;
                finalLight += (lights[i].color * lightRatio) * atten;
            }
            else
            {
                float3 dir = lights[i].pos - input.worldPos;
                if (length(dir) > lights[i].radius)
                    continue;

                dir = normalize(dir);
                float lightRatio = saturate(dot(dir, input.norm));
                float atten = 1.0f - saturate(length(lights[i].pos - input.worldPos) / lights[i].radius);
                atten *= atten;
                finalLight += (lights[i].color * lightRatio) * atten;
            }
        }
        else
        {
            float3 dir = normalize(lights[i].dir);
            dir = -dir;
            float lightRatio = saturate(dot(dir, input.norm));
	        //atten *= atten;
            finalLight += lights[i].color * lightRatio;
        }
    }
	//finalLight *= atten;
    float4 diffuseColor = diffuse.Sample(sample, input.uv) * diffuseFactor;
	float4 emissiveColor = emissive.Sample(sample, input.uv) * emissiveFactor;

	//float3 reflectionDir = reflect(lightDir, input.norm);
	//float3 dirToCam = normalize(cameraPos - lightPos);
	//float specScale = 0.3f * pow(saturate(dot(reflectionDir, dirToCam)), 1.0f);
	//float specIntense = specular.Sample(sample, input.uv).x;
	//float4 specColor = specIntense * specScale * lightColor;
	return (finalLight * diffuseColor) + emissiveColor;// + specColor;
}
