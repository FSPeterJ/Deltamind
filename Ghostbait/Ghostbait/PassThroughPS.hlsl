// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

struct PixelShaderOutput
{
        float4 diffuse : SV_TARGET0;
        float4 emissive : SV_TARGET1;
        float4 normal : SV_TARGET2;
        float4 specular : SV_TARGET3;
        float4 depth : SV_TARGET4;
        float4 unlit : SV_TARGET5;
};

// A pass-through function for the (interpolated) color data.
PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.diffuse = float4(input.color, 1.0f);
    output.emissive = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.normal = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.depth = float4(input.pos.z, input.pos.z, input.pos.z, input.pos.z);
    output.unlit = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}
