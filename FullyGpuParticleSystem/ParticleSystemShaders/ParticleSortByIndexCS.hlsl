#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbSortConstants : register(b0)
{
	uint sequenceSize;
	uint stage;
}

#define NUM_THREADS 256

[numthreads(NUM_THREADS, 1, 1)]
void BitonicSortCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint id = dispatchThreadId.x;
	uint compareIndex = id ^ stage;

	// prevent comparing twice.
	if (id < compareIndex)
	{
		// sort descending order
		// if want ascending order, you should just reverse comparison operator
		// please note that it doesn't work if you change like as following:
		// isDescneding = ((id & sequenceSize) == 1);
		bool isAscending = ((id & sequenceSize) == 0);

		if ((isAscending && aliveIndices[id] < aliveIndices[compareIndex]) ||
			(!isAscending && aliveIndices[id] > aliveIndices[compareIndex]))
		{
			uint temp = aliveIndices[id];
			aliveIndices[id] = aliveIndices[compareIndex];
			aliveIndices[compareIndex] = temp;
		}
	}
}