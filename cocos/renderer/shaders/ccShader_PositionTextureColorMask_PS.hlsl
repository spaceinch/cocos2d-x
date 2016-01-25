Texture2D g_Texture0;
SamplerState g_Sampler0: register(s0);

cbuffer ConstantBuffer : register(b0)
{
	Texture2D u_mask;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 texUV : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 tex = g_Texture0(g_Sampler0, input.texUV);

	float maskAlpha = u_mask(g_Sampler0, input.texUV).a;
	float texAlpha = tex.a;
	float blendAlpha = maskAlpha * texAlpha; // Show only where mask is not visible

	return float4(tex.rgb, blendAlpha);
}
