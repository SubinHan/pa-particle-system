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

		float positionX;
		float positionY;
		float positionZ;
		float velocityX;
		float previousPositionX;
		float previousPositionY;
		float previousPositionZ;
		float previousVeloictyX;

		unpackUintToFloat2(particlesCurrent[id].PositionXY, positionX, positionY);
		unpackUintToFloat2(particlesCurrent[id].PositionZVelocitiyX, positionZ, velocityX);
		unpackUintToFloat2(particlesCurrent[previousId].PositionXY, previousPositionX, previousPositionY);
		unpackUintToFloat2(particlesCurrent[previousId].PositionZVelocityX, previousPositionZ, previousVelocityX);

		float distanceFromPrevious = 
			length(
				float3(positionX, positionY, positionZ) - 
				float3(previousPositionX, previousPositionY, previousPositionZ));

		particlesCurrent[id].DistanceFromPreviousAndDistanceFromStart =
			packFloat2ToUint(distanceFromPrevious, distanceFromPrevious);
	}
}