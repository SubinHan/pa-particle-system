#include "Core/ParticleComputePass.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/ShaderStatementGraph.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include "d3dx12.h"

constexpr int ROOT_SLOT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK = ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT + 1;
constexpr int ROOT_SLOT_DEADS_INDICES_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK + 1;
constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_DEADS_INDICES_BUFFER + 1;

ParticleComputePass::ParticleComputePass(ParticleResource* resource, std::string name) :
	ParticlePass(resource, name)
{
}

void ParticleComputePass::setParticlesComputeRootUav(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetComputeRootUnorderedAccessView(ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK, _resource->getAliveIndicesResourceBack()->GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(ROOT_SLOT_DEADS_INDICES_BUFFER, _resource->getDeadIndicesResource()->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersUavGpuHandle());
}

void ParticleComputePass::bindComputeResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot)
{
	if (!_shaderStatementGraph)
		return;

	for (int i = 0; i < _shaderStatementGraph->getSize(); ++i)
	{
		int numResourcesToBind = _shaderStatementGraph->getNode(i)->getNumResourcesToBind();

		if (numResourcesToBind == 0)
			continue;

		for (int j = 0; j < numResourcesToBind; ++j)
		{
			if (_shaderStatementGraph->getNode(i)->isResourceViewCreated(j))
				commandList->SetComputeRootDescriptorTable(startRootSlot, _shaderStatementGraph->getNode(i)->getResourceGpuHandle(j));
			startRootSlot++;
		}
	}
}

void ParticleComputePass::setConstants(ID3D12GraphicsCommandList* commandList, const void* constants)
{
	commandList->SetComputeRoot32BitConstants(ROOT_SLOT_CONSTANTS_BUFFER, getNum32BitsConstantsUsing(), constants, 0);
}

void ParticleComputePass::readyDispatch(ID3D12GraphicsCommandList* commandList)
{
	if (isShaderDirty())
	{
		buildRootSignature();
		rebuildComputePsos();
		setShaderDirty(false);
	}

	commandList->SetComputeRootSignature(getRootSignature());
	commandList->SetPipelineState(_pso.Get());
	setParticlesComputeRootUav(commandList);
	bindComputeResourcesOfRegisteredNodes(commandList, ROOT_SLOT_COUNTERS_BUFFER + 1);
}

void ParticleComputePass::setComputeShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_computeShader = shader;
	setShaderDirty(true);
}

std::vector<CD3DX12_ROOT_PARAMETER> ParticleComputePass::buildRootParameter()
{
	// root parameter for renderbase
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(6);
	slotRootParameter[ROOT_SLOT_CONSTANTS_BUFFER].InitAsConstants(getNum32BitsConstantsUsing(), 0);

	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER]
		.InitAsUnorderedAccessView(0);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT]
		.InitAsUnorderedAccessView(1);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK]
		.InitAsUnorderedAccessView(2);
	slotRootParameter[ROOT_SLOT_DEADS_INDICES_BUFFER]
		.InitAsUnorderedAccessView(3);

	_counterUavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 4);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER]
		.InitAsDescriptorTable(1, &_counterUavTable);

	return slotRootParameter;
}

int ParticleComputePass::getNumSrvUsing()
{
	return 0;
}

int ParticleComputePass::getNumUavUsing()
{
	return 5;
}

bool ParticleComputePass::needsStaticSampler()
{
	return false;
}

ID3DBlob* ParticleComputePass::getComputeShader() const
{
	return _computeShader.Get();
}

void ParticleComputePass::rebuildComputePsos()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = getRootSignature();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(getComputeShader()->GetBufferPointer()),
		getComputeShader()->GetBufferSize()
	};
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	auto device = DxDevice::getInstance().getD3dDevice();
	ThrowIfFailed(
		device->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_pso)));
}
