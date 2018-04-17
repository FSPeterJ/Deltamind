struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer BrightnessFactor : register(b0)
{
    float Gamma;
};

float4 main() : SV_TARGET
{
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}