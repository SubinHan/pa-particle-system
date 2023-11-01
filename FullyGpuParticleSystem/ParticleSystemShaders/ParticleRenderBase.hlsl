#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleSystem.hlsl"
#include "ParticleSystemShaders/Util.hlsl"

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
	uint ThreadId : THREADID;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	uint ThreadId : THREADID;
};

StructuredBuffer<Particle> particles : register(t0);
StructuredBuffer<uint> aliveIndices : register(t1);

%t

%u

SamplerState gsamPointWrap  : register(s0);
SamplerState gsamPointClamp  : register(s1);
SamplerState gsamLinearWrap  : register(s2);
SamplerState gsamLinearClamp  : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp  : register(s5);

VertexOut ParticleVS(
	uint vid : SV_VertexID)
{
	const uint threadId = vid;

	const uint particleIndex = aliveIndices[threadId];
	Particle particle = particles[particleIndex];

	VertexOut vertexOut;

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
void ParticleGS(
	point VertexOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosW - gin[0].CenterW;

	float3 u = normalize(cross(look, up));
	float3 v = normalize(cross(look, u));

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

	GeoOut geoOut;
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

float4 ParticlePS(GeoOut pin) : SV_Target
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	const uint particleIndex = aliveIndices[pin.ThreadId];
	Particle particle = particles[particleIndex];

	float initialLifetime = particle.InitialLifetime;
	float remainLifetime = particle.RemainLifetime;
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float3 initialColor = particle.InitialColor;
	float3 endColor = particle.EndColor;
	float3 interpolatedColor = lerp(initialColor, endColor, normalizedLifetimeInv);

	float initialOpacity = particle.InitialOpacity;
	float endOpacity = particle.EndOpacity;
	float interpolatedOpacity = lerp(initialOpacity, endOpacity, normalizedLifetimeInv);

	color = float4(interpolatedColor, interpolatedOpacity);

	%s

	return color;
}