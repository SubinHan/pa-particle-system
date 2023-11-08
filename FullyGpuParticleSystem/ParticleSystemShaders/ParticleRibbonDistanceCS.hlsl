#include "ParticleSystemShaders/Particle.hlsl"

cbuffer cbPreRibbonDistanceConstants : register(b0)
{
    uint NumParticles;
}

RWStructuredBuffer<Particle> particles : register(u0);
RWStructuredBuffer<uint> aliveIndices : register(u1);

void addFromTo(uint from, uint to)
{
    //if (from >= NumParticles || to >= NumParticles)
    //    return;

    particles[aliveIndices[to]].DistanceFromStart += particles[aliveIndices[from]].DistanceFromStart;
}

[numthreads(256, 1, 1)]
void BrentKung(int3 dispatchThreadId : SV_GroupThreadID)
{
    //Upsweep
    if (dispatchThreadId.x < (NumParticles >> 1))
    {
        addFromTo(dispatchThreadId.x << 1, (dispatchThreadId.x << 1) + 1);
    }

    int offset = 1;
    for (int j = NumParticles >> 2; j > 0; j >>= 1)
    {
        DeviceMemoryBarrierWithGroupSync();
        if (dispatchThreadId.x < j)
        {
            addFromTo(
                (((dispatchThreadId.x << 1) + 1) << offset) - 1,
                (((dispatchThreadId.x << 1) + 2) << offset) - 1);
        }
        ++offset;
    }

    --offset;

    //Downsweep
    for (j = 2; j < NumParticles; j <<= 1)
    {
        --offset;
        DeviceMemoryBarrierWithGroupSync();
        if (dispatchThreadId.x < j - 1)
        {
            addFromTo(
                (((dispatchThreadId.x << 1) + 2) << offset) - 1,
                (((dispatchThreadId.x << 1) + 3) << offset) - 1);
        }
    }
}
