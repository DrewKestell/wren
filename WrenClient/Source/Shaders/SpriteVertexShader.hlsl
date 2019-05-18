cbuffer cbPerObject : register(b0)
{
	matrix gWorldViewProj;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float2 TexCoords : TEXCOORDS;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float2 TexCoords : TEXCOORDS;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogenous clip space
	vout.Pos = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

	// Pass texture coords through
	vout.TexCoords = vin.TexCoords;

	return vout;
}