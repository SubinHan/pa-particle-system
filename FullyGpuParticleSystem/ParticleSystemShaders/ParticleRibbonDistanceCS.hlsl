#include "ParticleSystemShaders/Util.hlsl"
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
    if (from == 0 || to == 0)
        return;

    uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

    if (from >= numAlives || to >= numAlives)
        return;

    float distanceFromPrevious;
    float distanceFromStart;
    unpackUintToFloat2(
        particlesCurrent[to].DistanceFromPreviousAndDistanceFromStart,
        distanceFromPrevious,
        distanceFromStart);

    float unused;
    float distanceFromStartAdder;
    unpackUintToFloat2(
        particlesCurrent[from].DistanceFromPreviousAndDistanceFromStart,
        unused,
        distanceFromStartAdder);

    distanceFromStart += distanceFromStartAdder;

    particlesCurrent[to].DistanceFromPreviousAndDistanceFromStart =
        packFloat2ToUint(distanceFromPrevious, distanceFromStart);
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
