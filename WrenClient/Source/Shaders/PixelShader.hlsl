Texture2D GTexture : register(t0);
SamplerState GSampler : register(s0);

cbuffer cbPerFrame : register(b0)
{
	float4 directionalLightPos;
	float3 directionalLightColor;
};

struct PixelInput
{
	float4 Pos : SV_POSITION;
	nointerpolation float4 Normal : NORMAL;
	float2 TexCoords : TEXCOORDS;
};

float4 main(PixelInput pin) : SV_TARGET
{
	return float4(GTexture.Sample(GSampler, pin.TexCoords) * directionalLightColor, 1.0f);

	/*float4 ambient = float4(0.8f, 0.8f, 0.8f, 1.0f) * GTexture.Sample(GSampler, pin.TexCoords);
	float4 diffuse = float4(directionalLightColor, 1.0f) * saturate(max(0, dot(-directionalLightPos, pin.Normal))) * GTexture.Sample(GSampler, pin.TexCoords);
	return ambient + diffuse;*/
}