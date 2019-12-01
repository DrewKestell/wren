Texture2D GTexture : register(t0);
SamplerState GSampler : register(s0);

cbuffer cbPerFrame : register(b0)
{
	float4 directionalLightPos;
	float3 directionalLightColor;
	float pad;
	float directionalLightIntensity;
	float ambientIntensity;
};

struct PixelInput
{
	float4 Pos : SV_POSITION;
	nointerpolation float4 Normal : NORMAL;
	float2 TexCoords : TEXCOORDS;
};

float4 main(PixelInput pin) : SV_TARGET
{
	float4 ambient = float4(directionalLightColor * ambientIntensity, 1.0f) * GTexture.Sample(GSampler, pin.TexCoords);
	float4 diffuse = float4(directionalLightColor * directionalLightIntensity, 1.0f) * saturate(max(0, dot(-directionalLightPos, pin.Normal))) * GTexture.Sample(GSampler, pin.TexCoords);
	return ambient + diffuse;
}