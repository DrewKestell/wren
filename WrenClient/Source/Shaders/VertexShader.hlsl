cbuffer cbPerObject : register(b0)
{
	matrix gWorldViewProj;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoords : TEXCOORDS;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 TexCoords : TEXCOORDS;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogenous clip space
	vout.Pos = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

	// Pass normal vector through
	vout.Normal = normalize(float4(vin.Normal, 0.0f));

	// Pass texture coords through
	vout.TexCoords = vin.TexCoords;

	return vout;
}