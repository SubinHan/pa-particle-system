#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

[numthreads(256, 1, 1)]
void PreRibbonDistanceCS(
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);
	const uint id = dispatchThreadId.x;

	if (id < numAlives)
	{
		uint previousId;
		if (id == 0)
			previousId = 0;
		else
			previousId = dispatchThreadId.x - 1;

		const uint particleIndex = aliveIndices[id];
		const uint previousParticleIndex = aliveIndices[previousId];

		float distanceFromPrevious = 
			length(particles[particleIndex].Position - particles[previousParticleIndex].Position);

		particles[particleIndex].DistanceFromPrevious = distanceFromPrevious;
		particles[particleIndex].DistanceFromStart = distanceFromPrevious;

	}

	if (id == 0)
	{
		particles[0].DistanceFromStart = 0.0f;
	}
}