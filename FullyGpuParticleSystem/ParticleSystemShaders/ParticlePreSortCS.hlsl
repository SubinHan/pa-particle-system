#include "ParticleSystemShaders/Particle.hlsl"

cbuffer cbPreSortConstants : register(b0)
{
	uint fillEndIndex;
	uint fillNumber;
}

RWStructuredBuffer<uint> aliveIndices	: register(u0);
RWByteAddressBuffer counters			: register(u1);

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