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

float4 ParticlePS(GeoOut pin) : SV_Target
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	% s

	return color;
}