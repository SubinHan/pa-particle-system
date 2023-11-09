#include "Core/ParticleRenderPass.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

void ParticleRenderPass::bindGraphicsResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot)
{
	for (int i = 0; i < _registeredNodes.size(); ++i)
	{
		int numResourcesToBind = _registeredNodes[i]->getNumResourcesToBind();

		if (numResourcesToBind == 0)
			continue;

		for (int j = 0; j < numResourcesToBind; ++j)
		{
			if (_registeredNodes[i]->isResourceViewCreated(j))
				commandList->SetGraphicsRootDescriptorTable(startRootSlot, _registeredNodes[i]->getResourceGpuHandle(j));
			startRootSlot++;
		}
	}
}

void ParticleRenderPass::setVerticalShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderVs = shader;
	_isShaderDirty = true;
}

void ParticleRenderPass::setHullShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderHs = shader;
	_isShaderDirty = true;
}

void ParticleRenderPass::setDomainShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderDs = shader;
	_isShaderDirty = true;
}

void ParticleRenderPass::setGeometryShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderGs = shader;
	_isShaderDirty = true;
}

void ParticleRenderPass::setPixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderPs = shader;
	_isShaderDirty = true;
}
