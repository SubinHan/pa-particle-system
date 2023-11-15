#include "Core/ParticlePass.h"

#include "Core/DxDevice.h"
#include "Core/ShaderStatementGraph.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "Model/ResourceViewType.h"
#include "Util/DxUtil.h"

ParticlePass::ParticlePass(ParticleResource* resource, std::string name) :
	Hashable(),
	_resource(resource),
	_name(name),
	_shaderStatementGraph()
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

void ParticlePass::setShaderStatementGraph(std::shared_ptr<ShaderStatementGraph> graph)
{
	_shaderStatementGraph = graph;
	onShaderStatementGraphChanged();
}

std::shared_ptr<ShaderStatementGraph> ParticlePass::getShaderStatementGraph()
{
	return _shaderStatementGraph;
}

bool ParticlePass::isShaderDirty() const
{
	return _isShaderDirty;
}

void ParticlePass::setShaderDirty(bool newIsShaderDirty)
{
	_isShaderDirty = newIsShaderDirty;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> getStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

void ParticlePass::buildRootSignature()
{
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter = buildRootParameter();
	std::vector<CD3DX12_DESCRIPTOR_RANGE> tables;

	int curRootParameterSlot = slotRootParameter.size();
	int curSrvRegister = getNumSrvUsing();
	int curUavRegister = getNumUavUsing();

	if (_shaderStatementGraph)
	{
		for (int i = 0; i < _shaderStatementGraph->getSize(); ++i)
		{
			const int numResourceToBind = _shaderStatementGraph->getNode(i)->getNumResourcesToBind();

			if (numResourceToBind == 0)
				continue;

			for (int j = 0; j < numResourceToBind; ++j)
			{
				if (_shaderStatementGraph->getNode(i)->getResourceViewType(j) == ResourceViewType::Srv)
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
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;

	auto staticSamplers = getStaticSamplers();

	if (needsStaticSampler())
	{
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

	auto& device = DxDevice::getInstance();

	ThrowIfFailed(device.getD3dDevice()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.GetAddressOf())));
}