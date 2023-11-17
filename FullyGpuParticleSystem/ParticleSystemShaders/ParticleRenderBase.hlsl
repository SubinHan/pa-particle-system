#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/ParticleRenderPass.hlsl"

%t

RWStructuredBuffer<Particle> particles : register(u0);
RWStructuredBuffer<uint> aliveIndices : register(u1);
RWByteAddressBuffer counters			: register(u2);

%u

SamplerState gsamPointWrap  : register(s0);
SamplerState gsamPointClamp  : register(s1);
SamplerState gsamLinearWrap  : register(s2);
SamplerState gsamLinearClamp  : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp  : register(s5);

struct SpritePixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	uint ThreadId : THREADID;
};

float4 ParticlePS(SpritePixelIn pin) : SV_Target
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

struct RibbonPixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	float2 LocalTexC : LOCALTEXCOORD;
	uint ThreadId : THREADID;
};

float4 RibbonParticlePS(RibbonPixelIn pin) : SV_Target
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

	// previous particle
	uint previousThreadId = pin.ThreadId - 1;
	if (pin.ThreadId == 0)
	{
		previousThreadId = 0;
	}

	const uint previousParticleIndex = aliveIndices[previousThreadId];
	Particle previousParticle = particles[previousParticleIndex];

	float previousInitialLifetime = previousParticle.InitialLifetime;
	float previousRemainLifetime = previousParticle.RemainLifetime;
	float previousNormalizedLifetimeInv = (previousInitialLifetime - previousRemainLifetime) / previousInitialLifetime;

	float3 previousInitialColor = previousParticle.InitialColor;
	float3 previousEndColor = previousParticle.EndColor;
	float3 previousInterpolatedColor = lerp(previousInitialColor, previousEndColor, previousNormalizedLifetimeInv);

	float previousInitialOpacity = previousParticle.InitialOpacity;
	float previousEndOpacity = previousParticle.EndOpacity;
	float previousInterpolatedOpacity = lerp(previousInitialOpacity, previousEndOpacity, previousNormalizedLifetimeInv);

	interpolatedColor = lerp(previousInterpolatedColor, interpolatedColor, pin.LocalTexC.x);
	interpolatedOpacity = lerp(previousInterpolatedOpacity, interpolatedOpacity, pin.LocalTexC.x);

	color = float4(interpolatedColor, interpolatedOpacity);

	%s

	return color;
}
