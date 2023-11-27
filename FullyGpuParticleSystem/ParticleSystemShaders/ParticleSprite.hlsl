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
	float NormalizedLifetimeInv : NORMALIZEDLIFETIMEINV;
};

struct SpritePixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	float4 Color : COLOR;
	float NormalizedLifetimeInv : NORMALIZEDLIFETIMEINV;
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
	vertexOut.NormalizedLifetimeInv = normalizedLifetimeInv;

	return vertexOut;
}

#ifndef BOUNDING
[maxvertexcount(4)]
void SpriteParticleGS(
	point SpriteVertexOut gin[1],
	inout TriangleStream<SpritePixelIn> triStream)
{
	float4 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
	float3 up = normalize(mul(upView, gInvView).xyz);
	float3 normal = normalize(gEyePosW - gin[0].CenterW);

	// be careful that directx uses left-handed coordinate.
	float3 u = cross(normal, up);
	float3 v = cross(normal, u);

	float halfWidth = 0.5f * gin[0].Size;
	float halfHeight = 0.5f * gin[0].Size;

	//
	//	(0,0) - (1,0)
	//	  -		  -
	//	(0,1) - (1,1)
	//

	float4 vertices[4];
	vertices[0] = float4(gin[0].CenterW - halfWidth * u - halfHeight * v, 1.0f);
	vertices[1] = float4(gin[0].CenterW + halfWidth * u - halfHeight * v, 1.0f);
	vertices[2] = float4(gin[0].CenterW - halfWidth * u + halfHeight * v, 1.0f);
	vertices[3] = float4(gin[0].CenterW + halfWidth * u + halfHeight * v, 1.0f);

	float2 texC[4] =
	{
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f)
	};

	SpritePixelIn geoOut;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		geoOut.PosH = mul(vertices[i], gViewProj);
		geoOut.PosW = vertices[i].xyz;
		geoOut.NormalW = normal;
		geoOut.TexC = texC[i];
		geoOut.Color = gin[0].Color;
		geoOut.NormalizedLifetimeInv = gin[0].NormalizedLifetimeInv;

		triStream.Append(geoOut);
	}
}
#else
cbuffer cbBoundingConstants : register(b2)
{
	float2 BoundingVertices0;
	float2 BoundingVertices1;
	float2 BoundingVertices2;
	float2 BoundingVertices3;
	float2 BoundingVertices4;
	float2 BoundingVertices5;
	float2 BoundingVertices6;
	float2 BoundingVertices7;
}

[maxvertexcount(16)]
void SpriteParticleGS(
	point SpriteVertexOut gin[1],
	inout TriangleStream<SpritePixelIn> triStream)
{
	float4 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
	float3 up = mul(upView, gInvView).xyz;
	float3 normal = normalize(gEyePosW - gin[0].CenterW);

	// be careful that directx uses left-handed coordinate.
	float3 u = cross(normal, up);
	float3 v = cross(normal, u);

	float width = gin[0].Size;
	float height = gin[0].Size;
	
	float2 boundingVertices[9];

	int i = 0;
	boundingVertices[0] = BoundingVertices0;
	boundingVertices[1] = BoundingVertices1;
	boundingVertices[2] = BoundingVertices2;
	boundingVertices[3] = BoundingVertices3;
	boundingVertices[4] = BoundingVertices4;
	boundingVertices[5] = BoundingVertices5;
	boundingVertices[6] = BoundingVertices6;
	boundingVertices[7] = BoundingVertices7;
	boundingVertices[8] = BoundingVertices0;

	float2 offsetFromCenter[9];
	[unroll]
	for (i = 0; i < 9; ++i)
	{
		offsetFromCenter[i] = boundingVertices[i] - float2(0.5f, 0.5f);
	}

	float4 center = float4(gin[0].CenterW, 1.0f);

	float4 boundingVerticesW[9];
	[unroll]
	for (i = 0; i < 8; ++i)
	{
		boundingVerticesW[i] = float4(
			gin[0].CenterW + width * offsetFromCenter[i].x * u + height * offsetFromCenter[i].y * v, 
			1.0f);
	}
	boundingVerticesW[8] = boundingVerticesW[0];

	SpritePixelIn geoOut;
	[unroll]
	for (i = 0; i < 4; ++i)
	{
		geoOut.PosH = mul(boundingVerticesW[i * 2], gViewProj);
		geoOut.PosW = boundingVerticesW[i * 2].xyz;
		geoOut.NormalW = normal;
		geoOut.TexC = boundingVertices[i * 2];
		geoOut.Color = gin[0].Color;
		geoOut.NormalizedLifetimeInv = gin[0].NormalizedLifetimeInv;
		triStream.Append(geoOut);

		geoOut.PosH = mul(boundingVerticesW[i * 2 + 1], gViewProj);
		geoOut.PosW = boundingVerticesW[i * 2 + 1].xyz;
		geoOut.NormalW = normal;
		geoOut.TexC = boundingVertices[i * 2 + 1];
		geoOut.Color = gin[0].Color;
		geoOut.NormalizedLifetimeInv = gin[0].NormalizedLifetimeInv;
		triStream.Append(geoOut);

		geoOut.PosH = mul(center, gViewProj);
		geoOut.PosW = center.xyz;
		geoOut.NormalW = normal;
		geoOut.TexC = float2(0.5f, 0.5f);
		geoOut.Color = gin[0].Color;
		geoOut.NormalizedLifetimeInv = gin[0].NormalizedLifetimeInv;
		triStream.Append(geoOut);

		geoOut.PosH = mul(boundingVerticesW[i * 2 + 2], gViewProj);
		geoOut.PosW = boundingVerticesW[i * 2 + 2].xyz;
		geoOut.NormalW = normal;
		geoOut.TexC = boundingVertices[i * 2 + 2];
		geoOut.Color = gin[0].Color;
		geoOut.NormalizedLifetimeInv = gin[0].NormalizedLifetimeInv;
		triStream.Append(geoOut);

		triStream.RestartStrip();
	}
}
#endif