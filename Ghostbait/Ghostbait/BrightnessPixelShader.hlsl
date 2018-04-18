struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer BrightnessFactor : register(b5)
{
    float Gamma;
};

float4 main() : SV_TARGET
{
	return float4(Gamma, Gamma, Gamma, 1.0f);
}