Texture2D tex : register(t0);

SamplerState sample;

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer blurInfo : register(b3)
{
    float2 dir;
    float width;
    float height;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    static float offsets[] = { 0.0, 1.3846153846, 3.2307692308 };
    static float weights[] = { 0.2270270270, 0.3162162162, 0.0702702703 };

    float4 originalColor = tex.Sample(sample, input.uv) * weights[0];
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float2 position;
    for (int index = 1; index < 3; ++index)
    {
        position.x = input.uv.x + (offsets[index] * width * dir.x);
        position.y = input.uv.y + (offsets[index] * height * dir.y);
        color += (tex.Sample(sample, saturate(position)) * weights[index]).xyz;
        position.x = input.uv.x - (offsets[index] * width * dir.x);
        position.y = input.uv.y - (offsets[index] * height * dir.y);
        color += (tex.Sample(sample, saturate(position)) * weights[index]).xyz;
    }
    originalColor.xyz += color;
    return float4(originalColor.xyz, 1.0f);
}