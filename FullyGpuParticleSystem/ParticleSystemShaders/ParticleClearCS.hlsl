#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

[numthreads(1, 1, 1)]
void ClearCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	counters.Store(PARTICLECOUNTER_OFFSET_NEW_NUMALIVES, 0);
	counters.Store(PARTICLECOUNTER_OFFSET_NUMALIVES, 0);
	counters.Store(PARTICLECOUNTER_OFFSET_NUMSURVIVED, 0);
}