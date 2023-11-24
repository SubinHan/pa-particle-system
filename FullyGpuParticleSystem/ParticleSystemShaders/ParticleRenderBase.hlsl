#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/ParticleRenderPass.hlsl"

%t

RWStructuredBuffer<Particle> particles  : register(u0);
RWByteAddressBuffer counters			: register(u1);

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
	float4 Color : COLOR;
	float NormalizedLifetimeInv : NORMALIZEDLIFETIMEINV;
};

float4 ParticlePS(SpritePixelIn pin) : SV_Target
{
	float4 color = pin.Color;
	clip(color.a - 0.1f);

	float4 interpolatedColor = pin.Color;
	float normalizedLifetimeInv = pin.NormalizedLifetimeInv;

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
	nointerpolation uint ThreadId : THREADID;
};

float4 RibbonParticlePS(RibbonPixelIn pin) : SV_Target
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	Particle particle = particles[pin.ThreadId];

	float initialLifetime;
	float remainLifetime;

	unpackUintToFloat2(particle.InitialLifetimeAndRemainLifetime, initialLifetime, remainLifetime);
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float4 initialColor = unpackUintToUnorm4(particle.InitialColor);
	float4 endColor = unpackUintToUnorm4(particle.EndColor);
	float4 interpolatedColor = lerp(initialColor, endColor, normalizedLifetimeInv);

	// previous particle
	uint previousThreadId = pin.ThreadId - 1;
	if (pin.ThreadId == 0)
	{
		previousThreadId = 0;
	}

	Particle previousParticle = particles[previousThreadId];

	float previousInitialLifetime;
	float previousRemainLifetime;
	unpackUintToFloat2(
		previousParticle.InitialLifetimeAndRemainLifetime, 
		previousInitialLifetime,
		previousRemainLifetime);
	float previousNormalizedLifetimeInv = (previousInitialLifetime - previousRemainLifetime) / previousInitialLifetime;

	float4 previousInitialColor = unpackUintToUnorm4(previousParticle.InitialColor);
	float4 previousEndColor = unpackUintToUnorm4(previousParticle.EndColor);
	float4 previousInterpolatedColor = lerp(previousInitialColor, previousEndColor, previousNormalizedLifetimeInv);

	interpolatedColor = lerp(previousInterpolatedColor, interpolatedColor, pin.LocalTexC.x);

	color = interpolatedColor;

	%s

	return color;
}
