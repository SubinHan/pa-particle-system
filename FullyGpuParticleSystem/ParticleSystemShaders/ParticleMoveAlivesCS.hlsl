#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

#define NUM_THREADS 256

cbuffer cbUpdateConstants : register(b0)
{
	uint NumParticlesMayBeExpired;
	float DeltaTime;
}

[numthreads(NUM_THREADS, 1, 1)]
void MoveAlivesCS(
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlivesBeforeFrame =
		counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	// the number of survived particles in destroyCS phase before.
	uint numSurvived =
		counters.Load(PARTICLECOUNTER_OFFSET_NUMSURVIVED);

	// the number of destroyed particles in destroyCS phase before.
	uint numDestroyed =
		min(numAlivesBeforeFrame, NumParticlesMayBeExpired) - numSurvived;

	const uint id = NumParticlesMayBeExpired + dispatchThreadId.x;
	uint newNumAlives = numAlivesBeforeFrame - numDestroyed;

	if (id < numAlivesBeforeFrame)
	{
		uint newIndex = numSurvived + dispatchThreadId.x;

		Particle current = particlesCurrent[id];

		float initialLifetime;
		float remainLifetime;
		unpackUintToFloat2(
			current.InitialLifetimeAndRemainLifetime,
			initialLifetime,
			remainLifetime);
		remainLifetime -= DeltaTime;
		current.InitialLifetimeAndRemainLifetime =
			packFloat2ToUint(initialLifetime, remainLifetime);

		particlesNext[newIndex] = current;
	}

	if (dispatchThreadId.x == 0)
	{
		counters.Store(PARTICLECOUNTER_OFFSET_NEW_NUMALIVES, newNumAlives);
	}
}