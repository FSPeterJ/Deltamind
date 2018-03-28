Texture2D tex : register(t0);

SamplerState sample;

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float offsets[] = {1.0f, 2.0f, 3.0f, 4.0f };
    float weights[] = {4.0f, 3.0f, 2.0f, 1.0f };

    float sum = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        sum += weights[i] * 2.0f;
    }
    for (int j = 0; j < 4; ++j)
    {
        weights[j] /= sum;
    }//Normalizing the weights

    float3 color = float3(0.0f, 0.0f, 0.0f);
    for (int index = 0; index < 4; ++index)
    {
        color += (tex.Sample(sample, saturate(input.uv + offsets[index])) * weights[index]).xyz + (tex.Sample(sample, saturate(input.uv - offsets[index])) * weights[index]).xyz;
    }

    return float4(color, 1.0f);
}