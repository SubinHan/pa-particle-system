#include "Core/ParticlePass.h"

#include "Core/DxDevice.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "Model/ResourceViewType.h"

ParticlePass::ParticlePass(ParticleResource* resource, std::string name) :
	Hashable(),
	_resource(resource),
	_name(name)
{
}

std::string ParticlePass::getName()
{
	return _name;
}

ID3D12RootSignature* ParticlePass::getRootSignature()
{
	return _rootSignature.Get();
}

ID3DBlob* ParticlePass::getShader()
{
	return _shader.Get();
}

ID3D12PipelineState* ParticlePass::getPipelineStateObject()
{
	return _pso.Get();
}

void ParticlePass::registerShaderStatementNode(std::shared_ptr<ShaderStatementNode> node)
{
	_registeredNodes.push_back(node);
}

void ParticlePass::clearRegisteredShaderStatementNodes()
{
	_registeredNodes.clear();
}

void ParticlePass::setShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shader = shader;
	buildRootSignature();
	buildPsos();
}

void ParticlePass::bindComputeResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot)
{
	for (int i = 0; i < _registeredNodes.size(); ++i)
	{
		int numResourcesToBind = _registeredNodes[i]->getNumResourcesToBind();

		if (numResourcesToBind == 0)
			continue;

		for (int j = 0; j < numResourcesToBind; ++j)
		{
			if (_registeredNodes[i]->isResourceViewCreated(j))
				commandList->SetComputeRootDescriptorTable(startRootSlot, _registeredNodes[i]->getResourceGpuHandle(j));
			startRootSlot++;
		}
	}
}

void ParticlePass::bindGraphicsResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot)
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


void ParticlePass::buildRootSignature()
{
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter = buildRootParameter();
	std::vector<CD3DX12_DESCRIPTOR_RANGE> tables;

	int curRootParameterSlot = slotRootParameter.size();
	int curSrvRegister = getNumSrvUsing();
	int curUavRegister = getNumUavUsing();

	for (int i = 0; i < _registeredNodes.size(); ++i)
	{
		const int numResourceToBind = _registeredNodes[i]->getNumResourcesToBind();

		if (numResourceToBind == 0)
			continue;

		for (int j = 0; j < numResourceToBind; ++j)
		{
			if (_registeredNodes[i]->getResourceViewType(j) == ResourceViewType::Srv)
			{
				tables.emplace_back();

				int index = tables.size() - 1;
				tables[index].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, curSrvRegister++);
			}
		}
	}

	for (int i = 0; i < tables.size(); ++i)
	{
		slotRootParameter.emplace_back();
		slotRootParameter[curRootParameterSlot++].InitAsDescriptorTable(1, &tables[i]);
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;

	if (needsStaticSampler())
	{
		auto staticSamplers = DxUtil::getStaticSamplers();

		rootSigDesc = CD3DX12_ROOT_SIGNATURE_DESC(
			slotRootParameter.size(),
			slotRootParameter.data(),
			staticSamplers.size(),
			staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_NONE);
	}
	else
	{
		rootSigDesc = CD3DX12_ROOT_SIGNATURE_DESC(
			slotRootParameter.size(),
			slotRootParameter.data(),
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_NONE);
	}

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	auto device = DxDevice::getInstance();

	ThrowIfFailed(device.getD3dDevice()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.GetAddressOf())));
}