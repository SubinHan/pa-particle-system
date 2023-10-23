#include "Util.hlsl"
#include "ParticleApp/Shaders/Particle.hlsl"

cbuffer cbSortConstants : register(b0)
{
	uint sequenceSize;
	uint stage;
}

RWStructuredBuffer<uint> aliveIndices	: register(u0);

#define NUM_THREADS 256

// TODO: 현재 버그 있음.
// 특정 값들이 swap 대신 덮어쓰기 되어 몇몇 값들은 누락되고, 몇몇 값들은 복제되는 현상.

// each thread emits a particle.
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
		const bool isDescneding = ((id & sequenceSize) == 0);
		if ((isDescneding && aliveIndices[id] < aliveIndices[compareIndex]) ||
			(!isDescneding && aliveIndices[id] > aliveIndices[compareIndex]))
		{
			uint temp = aliveIndices[id];
			aliveIndices[id] = aliveIndices[compareIndex];
			aliveIndices[compareIndex] = temp;
		}
	}
}