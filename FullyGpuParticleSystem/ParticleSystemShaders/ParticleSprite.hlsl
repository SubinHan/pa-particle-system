#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/ParticleRenderPass.hlsl"

RWStructuredBuffer<Particle> particles : register(u0);
RWStructuredBuffer<uint> aliveIndices : register(u1);
RWByteAddressBuffer counters			: register(u2);

struct SpriteVertexOut
{
	float3 CenterW	: POSITION;
	float Size : SIZE;
	uint ThreadId : THREADID;
};

struct SpritePixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	uint ThreadId : THREADID;
};

SpriteVertexOut SpriteParticleVS(
	uint vid : SV_VertexID)
{
	const uint threadId = vid;

	const uint particleIndex = aliveIndices[threadId];
	Particle particle = particles[particleIndex];

	SpriteVertexOut vertexOut;

	const float4 posW = mul(float4(particle.Position, 1.0f), gWorld);
	vertexOut.CenterW = posW;

	float initialLifetime = particle.InitialLifetime;
	float remainLifetime = particle.RemainLifetime;
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float initialSize = particle.InitialSize;
	float endSize = particle.EndSize;
	float interpolatedSize = lerp(initialSize, endSize, normalizedLifetimeInv);

	vertexOut.Size = interpolatedSize;
	vertexOut.ThreadId = threadId;

	return vertexOut;
}

[maxvertexcount(4)]
void SpriteParticleGS(
	point SpriteVertexOut gin[1],
	inout TriangleStream<SpritePixelIn> triStream)
{
	float3 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
	float3 up = mul(upView, gInvView).xyz;
	float3 look = normalize(gEyePosW - gin[0].CenterW);

	float3 u = cross(look, up);
	float3 v = cross(look, u);

	float halfWidth = 0.5f * gin[0].Size;
	float halfHeight = 0.5f * gin[0].Size;

	float4 vertices[4];
	vertices[0] = float4(gin[0].CenterW + halfWidth * u - halfHeight * v, 1.0f);
	vertices[1] = float4(gin[0].CenterW + halfWidth * u + halfHeight * v, 1.0f);
	vertices[2] = float4(gin[0].CenterW - halfWidth * u - halfHeight * v, 1.0f);
	vertices[3] = float4(gin[0].CenterW - halfWidth * u + halfHeight * v, 1.0f);

	float2 texC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	SpritePixelIn geoOut;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		geoOut.PosH = mul(vertices[i], gViewProj);
		geoOut.PosW = vertices[i].xyz;
		geoOut.NormalW = look;
		geoOut.TexC = texC[i];
		geoOut.ThreadId = gin[0].ThreadId;

		triStream.Append(geoOut);
	}
}