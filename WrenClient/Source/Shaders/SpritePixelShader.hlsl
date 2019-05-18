Texture2D GTexture : register(t0);
SamplerState GSampler : register(s0);

struct PixelInput
{
	float4 Pos : SV_POSITION;
	float2 TexCoords : TEXCOORDS;
};

float4 main(PixelInput pin) : SV_TARGET
{
	return GTexture.Sample(GSampler, pin.TexCoords);
}