#include "Particle.hlsl"

cbuffer cbPass : register(b0)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
};

struct VertexOut
{
	float3 CenterW	: POSITION;
	float Size		: SIZE;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	uint PrimId : SV_PrimitiveID;
};

StructuredBuffer<Particle> particles : register(t0);
StructuredBuffer<uint> aliveIndices : register(t1);

VertexOut ParticleVS(
	uint vid : SV_VertexID)
{
	const uint particleIndex = aliveIndices[vid];
	Particle particle = particles[particleIndex];

	VertexOut vertexOut;

	vertexOut.CenterW = particle.Position;
	vertexOut.Size = particle.Size;

	return vertexOut;
}

[maxvertexcount(4)]
void ParticleGS(
	point VertexOut gin[1],
	uint primId : SV_PrimitiveID,
	inout TriangleStream<GeoOut> triStream)
{
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosW - gin[0].CenterW;
	
	float3 u = normalize(cross(up, look));
	float3 v = normalize(cross(u, look));

	float halfWidth = 0.5f * gin[0].Size;
	float halfHeight = 0.5f * gin[0].Size;

	float4 vertices[4];
	vertices[0] = float4(gin[0].CenterW + halfWidth * u - halfHeight * up, 1.0f);
	vertices[1] = float4(gin[0].CenterW + halfWidth * u + halfHeight * up, 1.0f);
	vertices[2] = float4(gin[0].CenterW - halfWidth * u - halfHeight * up, 1.0f);
	vertices[3] = float4(gin[0].CenterW - halfWidth * u + halfHeight * up, 1.0f);

	float2 texC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	GeoOut geoOut;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		geoOut.PosH = mul(vertices[i], gViewProj);
		geoOut.PosW = vertices[i].xyz;
		geoOut.NormalW = look;
		geoOut.TexC = texC[i];
		geoOut.PrimId = primId;

		triStream.Append(geoOut);
	}
}

float4 ParticlePS(GeoOut pin) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}