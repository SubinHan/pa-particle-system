#include "Core/ParticleComputePass.h"

#include "Core/ShaderStatementGraph.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

ParticleComputePass::ParticleComputePass(ParticleResource* resource, std::string name) :
	ParticlePass(resource, name)
{
}

void ParticleComputePass::bindComputeResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot)
{
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

void ParticleComputePass::setComputeShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_computeShader = shader;
	_isShaderDirty = true;
}
