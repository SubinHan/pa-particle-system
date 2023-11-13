#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbPreRibbonDistanceConstants : register(b0)
{
    uint NumWorker;
    uint IndexOffsetFrom;
    uint IndexOffsetTo;
    uint ShiftOffset;
}

void addFromTo(uint from, uint to)
{
    if (aliveIndices[from] == 0 || aliveIndices[to] == 0)
        return;

    uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

    if (from >= numAlives || to >= numAlives)
        return;

    particles[aliveIndices[to]].DistanceFromStart += particles[aliveIndices[from]].DistanceFromStart;
}

[numthreads(1024, 1, 1)]
void BrentKung(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (dispatchThreadId.x < NumWorker)
    {
        addFromTo(
            (((dispatchThreadId.x << 1) + IndexOffsetFrom) << ShiftOffset) - 1,
            (((dispatchThreadId.x << 1) + IndexOffsetTo) << ShiftOffset) - 1);
    }
}
