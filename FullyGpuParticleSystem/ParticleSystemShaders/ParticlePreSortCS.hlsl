#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbPreSortConstants : register(b0)
{
	uint fillEndIndex;
	uint fillNumber;
}

[numthreads(256, 1, 1)]
void PreSortCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	uint id = numAlives + dispatchThreadId.x;

	if (id < fillEndIndex)
	{
		aliveIndices[id] = fillNumber;
	}
}