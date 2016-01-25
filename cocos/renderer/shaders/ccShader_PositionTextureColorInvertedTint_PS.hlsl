Texture2D g_Texture0;
SamplerState g_Sampler0: register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 texUV : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 finalColor = g_Texture0.Sample(g_Sampler0, input.texUV);
	finalColor.rgb = finalColor.rgb + (float3(1, 1, 1) - finalColor.rgb) * input.color.rgb * finalColor.a;

	return finalColor;
}
