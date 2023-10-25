#include "Particle.hlsl"
#include "ParticleSystem.hlsl"

cbuffer cbPass : register(b1)
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
	float DeltaTime;
};

struct VertexOut
{
	float3 CenterW	: POSITION;
	float Size : SIZE;
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

Texture2D diffuseMap : register(t2);

SamplerState gsamPointWrap  : register(s0);
SamplerState gsamPointClamp  : register(s1);
SamplerState gsamLinearWrap  : register(s2);
SamplerState gsamLinearClamp  : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp  : register(s5);

VertexOut ParticleVS(
	uint vid : SV_VertexID)
{
	const uint particleIndex = aliveIndices[vid];
	Particle particle = particles[particleIndex];

	VertexOut vertexOut;

	const float4 posW = mul(float4(particle.Position, 1.0f), gWorld);
	vertexOut.CenterW = posW;
	vertexOut.Size = particle.Size;

	return vertexOut;
}