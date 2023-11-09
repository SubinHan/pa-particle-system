#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/Particle.hlsl"

cbuffer cbSortConstants : register(b0)
{
	uint particlesBufferSize;
	uint sequenceSize;
	uint stage;
}

RWStructuredBuffer<Particle> particles		: register(u0);

RWStructuredBuffer<uint> aliveIndices	: register(u1);
RWByteAddressBuffer counters			: register(u2);

#define NUM_THREADS 256

// if true, the order is ascending.
bool compare(Particle p1, Particle p2)
{
	if (p1.SpawnTime == p2.SpawnTime)
	{
		return p1.SpawnOrderInFrame < p2.SpawnOrderInFrame;
	}

	return p1.SpawnTime < p2.SpawnTime;
}

[numthreads(NUM_THREADS, 1, 1)]
void BitonicSortCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint id = dispatchThreadId.x;

	uint compareIndex = id ^ stage;

	uint numAlives =
		counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	// prevent comparing twice.
	if (id < compareIndex)
	{
		bool isAscending = ((id & sequenceSize) == 0);

		uint particleIndex1 = aliveIndices[id];
		uint particleIndex2 = aliveIndices[compareIndex];
		Particle p1 = particles[particleIndex1];
		Particle p2 = particles[particleIndex2];

		if ((isAscending && !compare(p1, p2)) ||
			(!isAscending && compare(p1, p2)))
		{
			uint temp = aliveIndices[id];
			aliveIndices[id] = aliveIndices[compareIndex];
			aliveIndices[compareIndex] = temp;
		}
	}
}