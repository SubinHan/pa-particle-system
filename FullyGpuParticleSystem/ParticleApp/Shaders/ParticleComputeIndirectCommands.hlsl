#include "ParticleApp/Shaders/Particle.hlsl"

struct IndirectCommand
{
	uint IndexCountPerInstance;
	uint InstanceCount;
	uint StartIndexLocation;
	int BaseVertexLocation;
	uint StartInstanceLocation;
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
	command.IndexCountPerInstance = 1;
	command.InstanceCount = 1;
	command.StartIndexLocation = id;
	command.BaseVertexLocation = 0;
	command.StartInstanceLocation = 0;

	outputCommands.Append(command);
}
