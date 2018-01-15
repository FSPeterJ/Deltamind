// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

texture2D tex : register(t0);

SamplerState sample : register(s0);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    return float4(0.0f, 0.5f, 0.5f, 1.0f);
    //return tex.Sample(sample, input.uv);
}