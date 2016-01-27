// **************************************
// Use: needs sample a texture with R16_UNORM format
// **************************************

Texture2D g_Texture0;
SamplerState g_Sampler0 : register(s0);

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ConstantBuffer : register(b0)
{
	float4 u_effectColor;
	float4 u_textColor;
};

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
	float4 sampledColor = g_Texture0.Sample(g_Sampler0, input.texUV);

	uint asDoubleByte = max(0.0, sampledColor.r * 65536.0); // Multiply by 2^32 for transform components values in 255 range ones
	uint highValue = asDoubleByte / 256.0; // Divide by 2^8 for shift 8 bits to the right and get component in second byte
	uint lowValue = (asDoubleByte * 16777216.0) / 16777216.0; // 2 ^ (8 * 3) Shift 3 bytes to the left then same to the right for isolate and get component in first position

	float outlineAlpha = lowValue;
	outlineAlpha /= 255.0;
	 
	float fontAlpha = highValue;
	fontAlpha /= 255.0;

	clip(fontAlpha + outlineAlpha);
	 
	float4 color = u_textColor * fontAlpha + u_effectColor * (1.0 - fontAlpha);
	 
	return input.color * float4(color.rgb, max(fontAlpha, outlineAlpha) * color.a);
}
