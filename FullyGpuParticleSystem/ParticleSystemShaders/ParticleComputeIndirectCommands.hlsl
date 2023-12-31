#include "ParticleSystemShaders/Particle.hlsl"

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

[numthreads(1, 1, 1)]
void ComputeIndirectCommandsCS(int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);
	
	IndirectCommand command;
	command.IndexCountPerInstance = 1;
	command.InstanceCount = numAlives;
	command.StartIndexLocation = 0;
	command.BaseVertexLocation = 0;
	command.StartInstanceLocation = 0;

	outputCommands.Append(command);
}

[numthreads(1, 1, 1)]
void RibbonComputeIndirectCommandsCS(int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint id = dispatchThreadId.x;
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	IndirectCommand command;
	command.IndexCountPerInstance = 4;

	uint instanceCount;
	if (numAlives == 0)
	{
		instanceCount = 0;
	}
	else
	{
		instanceCount = numAlives - 1;
	}

	command.InstanceCount = instanceCount;
	command.StartIndexLocation = id;
	command.BaseVertexLocation = 0;
	command.StartInstanceLocation = 0;

	outputCommands.Append(command);
}

//[numthreads(256, 1, 1)]
//void RibbonComputeIndirectCommandsCS(int3 dispatchThreadId : SV_DispatchThreadID)
//{
//	uint id = dispatchThreadId.x;
//	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);
//
//	if (id >= numAlives - 1 || numAlives <= 1)
//	{
//		return;
//	}
//
//	IndirectCommand command;
//	command.IndexCountPerInstance = 4;
//	command.InstanceCount = 1;
//	command.StartIndexLocation = id;
//	command.BaseVertexLocation = 0;
//	command.StartInstanceLocation = 0;
//
//	outputCommands.Append(command);
//}
