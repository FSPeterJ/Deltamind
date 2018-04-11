Texture2D tex : register(t0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState sample
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    MinLOD = 0;
    MaxLOD = 0;
};
float4 main(PixelShaderInput input) : SV_TARGET
{
    return tex.Sample(sample, input.uv);
}