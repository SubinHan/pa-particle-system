#include "ParticleApp/Shaders/Particle.hlsl"

struct IndirectCommand
{
	uint4 DrawArguments;
};

RWByteAddressBuffer counters : register(u0);
AppendStructuredBuffer<IndirectCommand> outputCommands : register(u1);

[numthreads(256, 1, 1)]
void ComputeIndirectCommandsCS(int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint id = dispatchThreadId.x;
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	if (id >= numAlives)
	{
		return;
	}
	
	IndirectCommand command;
	command.DrawArguments.x = 1;
	command.DrawArguments.y = 1;
	command.DrawArguments.z = id;
	command.DrawArguments.w = 0;

	outputCommands.Append(command);
}
