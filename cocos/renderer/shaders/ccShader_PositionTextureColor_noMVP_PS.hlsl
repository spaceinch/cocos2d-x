Texture2D g_Texture0;
SamplerState g_Sampler0 : register(s0);

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 texUV : TEXCOORD0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return input.color * g_Texture0.Sample(g_Sampler0, input.texUV);
}
