#include "ParticleSystemShaders/Particle.hlsl"

cbuffer cbPreSortConstants : register(b0)
{
	uint fillEndIndex;
	uint fillNumber;
}

RWStructuredBuffer<Particle> particles		: register(u0);

RWStructuredBuffer<uint> aliveIndices	: register(u1);
RWByteAddressBuffer counters			: register(u2);

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