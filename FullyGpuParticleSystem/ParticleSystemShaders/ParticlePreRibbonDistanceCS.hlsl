#include "ParticleSystemShaders/Particle.hlsl"
//
//cbuffer cbPreRibbonDistanceConstants : register(b0)
//{
//}

RWStructuredBuffer<Particle> particles		: register(u0);
RWStructuredBuffer<uint> aliveIndices	: register(u1);

[numthreads(256, 1, 1)]
void PreRibbonDistanceCS(
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	const uint id = dispatchThreadId.x;
	uint previousId = dispatchThreadId.x - 1;
	if (id == 0)
		previousId = 0;

	numAlives < id¸é ±×¸¸!!!!

	const uint particleIndex = aliveIndices[id];
	const uint previousParticleIndex = aliveIndices[previousId];

	particles[particleIndex].DistanceFromPrevious =
		length(particles[particleIndex].Position - particles[previousParticleIndex].Position);
	particles[particleIndex].DistanceFromStart = particles[particleIndex].DistanceFromPrevious;
}