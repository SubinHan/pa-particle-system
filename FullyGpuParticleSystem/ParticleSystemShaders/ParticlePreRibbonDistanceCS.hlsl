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

		float distanceFromPrevious = 
			length(particlesCurrent[id].Position - particlesCurrent[previousId].Position);

		particlesCurrent[id].DistanceFromPrevious = distanceFromPrevious;
		particlesCurrent[id].DistanceFromStart = distanceFromPrevious;
	}
}