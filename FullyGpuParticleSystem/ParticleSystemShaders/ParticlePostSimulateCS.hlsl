#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

// each thread updates a particle and kills if expired.
[numthreads(1, 1, 1)]
void PostSimulateCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint newNumAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE);
	counters.Store(PARTICLECOUNTER_OFFSET_NUMALIVES, newNumAlives);
	counters.Store(PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE, 0);
}