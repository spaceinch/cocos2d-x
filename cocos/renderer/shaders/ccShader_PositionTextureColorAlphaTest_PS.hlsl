// A constant buffer that stores the alpha value
cbuffer AlphaBuffer : register(b0)
{
	float CC_alpha_value;
}

Texture2D g_Texture0;

SamplerState TextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
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
	float4 texColor = g_Texture0.Sample(TextureSampler, input.texUV);

	//if (texColor.a <= CC_alpha_value)
		//clip(-1);
    clip(texColor.a - CC_alpha_value);

	return input.color * texColor;
}
