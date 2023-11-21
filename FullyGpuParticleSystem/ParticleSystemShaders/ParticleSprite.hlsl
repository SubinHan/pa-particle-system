#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/ParticleRenderPass.hlsl"

RWStructuredBuffer<Particle> particles	: register(u0);
RWByteAddressBuffer counters			: register(u1);

struct SpriteVertexOut
{
	float3 CenterW	: POSITION;
	float Size : SIZE;
	float4 Color : COLOR;
};

struct SpritePixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	float4 Color : COLOR;
};

SpriteVertexOut SpriteParticleVS(
	uint iid : SV_InstanceID)
{
	const uint threadId = iid;

	Particle particle = particles[threadId];

	SpriteVertexOut vertexOut;

	float positionX;
	float positionY;
	float positionZ;
	float velocityX;
	unpackUintToFloat2(particle.PositionXY, positionX, positionY);
	unpackUintToFloat2(particle.PositionZVelocityX, positionZ, velocityX);

	const float4 posW = mul(float4(positionX, positionY, positionZ, 1.0f), gWorld);
	vertexOut.CenterW = posW.xyz;

	float initialLifetime;
	float remainLifetime;
	unpackUintToFloat2(particle.InitialLifetimeAndRemainLifetime, initialLifetime, remainLifetime);
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float initialSize;
	float endSize;
	unpackUintToFloat2(particle.InitialSizeAndEndSize, initialSize, endSize);
	float interpolatedSize = lerp(initialSize, endSize, normalizedLifetimeInv);

	float4 initialColor = unpackUintToUnorm4(particle.InitialColor);
	float4 endColor = unpackUintToUnorm4(particle.EndColor);
	float4 interpolatedColor = lerp(initialColor, endColor, normalizedLifetimeInv);

	vertexOut.Size = interpolatedSize;
	vertexOut.Color = interpolatedColor;

	return vertexOut;
}

[maxvertexcount(4)]
void SpriteParticleGS(
	point SpriteVertexOut gin[1],
	inout TriangleStream<SpritePixelIn> triStream)
{
	float4 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
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
		geoOut.Color = gin[0].Color;

		triStream.Append(geoOut);
	}
}