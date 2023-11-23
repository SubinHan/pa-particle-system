#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbUpdateConstants : register(b0)
{
	uint NumParticlesMayBeExpired;
}

cbuffer cbCounters : register(b1)
{
	uint NumAlivesNext;
	uint NumAlivesBeforeFrame;
	uint NumSurvived;
}

[numthreads(1, 1, 1)]
void PostDestroyCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlivesBeforeFrame = NumAlivesBeforeFrame;
	uint numSurvived = NumSurvived;
	uint numDestroyed =
		min(numAlivesBeforeFrame, NumParticlesMayBeExpired) - numSurvived;
	uint newNumAlives = numAlivesBeforeFrame - numDestroyed;

	counters.Store(PARTICLECOUNTER_OFFSET_NUMALIVES, newNumAlives);
	counters.Store(PARTICLECOUNTER_OFFSET_NEW_NUMALIVES, 0);
	counters.Store(PARTICLECOUNTER_OFFSET_NUMSURVIVED, 0);
}