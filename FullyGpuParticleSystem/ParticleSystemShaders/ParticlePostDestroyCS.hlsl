#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

[numthreads(1, 1, 1)]
void PostDestroyCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint newNumAlives =
		counters.Load(PARTICLECOUNTER_OFFSET_NEW_NUMALIVES);
	counters.Store(PARTICLECOUNTER_OFFSET_NUMALIVES, newNumAlives);
	counters.Store(PARTICLECOUNTER_OFFSET_NUMSURVIVED, 0);
}