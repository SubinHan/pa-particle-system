#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbUpdateConstants : register(b0)
{
	uint NumParticlesMayBeExpired;
	float DeltaTime;
}

// each thread updates a particle and kills if expired.
[numthreads(256, 1, 1)]
void DestroyCS(
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);
	const uint id = dispatchThreadId.x;

	if (id < NumParticlesMayBeExpired && id < numAlives)
	{
		particlesCurrent[id].RemainLifetime -= DeltaTime;

		bool isAlive = particlesCurrent[id].RemainLifetime > 0.f;

		if (isAlive)
		{
			uint newIndex;
			counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMSURVIVED, 1, newIndex);

			particlesNext[newIndex] = particlesCurrent[id];
		}
	}
}