#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbSortConstants : register(b0)
{
	uint particlesBufferSize;
	uint sequenceSize;
	uint stage;
}

#define NUM_THREADS 256

// if true, the order is ascending.
bool compare(Particle p1, Particle p2)
{
	bool result;

	result = p1.SpawnTime < p2.SpawnTime;

	return result;
}

[numthreads(NUM_THREADS, 1, 1)]
void BitonicSortCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint id = dispatchThreadId.x;

	if (id >= particlesBufferSize)
	{
		return;
	}

	uint compareIndex = id ^ stage;

	// prevent comparing twice.
	if (id < compareIndex)
	{
		bool isAscending = ((id & sequenceSize) == 0);

		Particle p1 = particlesCurrent[id];
		Particle p2 = particlesCurrent[compareIndex];

		if ((isAscending && !compare(p1, p2)) ||
			(!isAscending && compare(p1, p2)))
		{
			Particle temp = particlesCurrent[id];
			particlesCurrent[id] = particlesCurrent[compareIndex];
			particlesCurrent[compareIndex] = temp;
		}
	}
}