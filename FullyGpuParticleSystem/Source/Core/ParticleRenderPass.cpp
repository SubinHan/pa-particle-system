#include "Core/ParticleRenderPass.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/ShaderStatementGraph.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "Core/PassConstantBuffer.h"
#include "Core/TextureManager.h"
#include "Model/ObjectConstants.h"
#include "Model/Geometry.h"

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_BOUNDING_CONSTANTS_BUFFER = ROOT_SLOT_COUNTERS_BUFFER + 1;
constexpr int ROOT_SLOT_SIZE = ROOT_SLOT_BOUNDING_CONSTANTS_BUFFER + 1;
constexpr int BOUNDING_POLYGON_N = 8;

ParticleRenderPass::ParticleRenderPass(ParticleResource* resource, std::string name) :
	ParticlePass(resource, name),
	_isOpaque(true),
	_isBounding(false),
	_isWireframe(false)
{
	buildComputeIndirectRootSignature();
	buildCommandSignature();
	generateEmptyGeometry();
}

ParticleRenderPass::~ParticleRenderPass() = default;

void ParticleRenderPass::bindGraphicsResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot)
{
	for (int i = 0; i < _shaderStatementGraph->getSize(); ++i)
	{
		int numResourcesToBind = _shaderStatementGraph->getNode(i)->getNumResourcesToBind();

		if (numResourcesToBind == 0)
			continue;

		for (int j = 0; j < numResourcesToBind; ++j)
		{
			if (_shaderStatementGraph->getNode(i)->isResourceViewCreated(j))
				commandList->SetGraphicsRootDescriptorTable(startRootSlot, _shaderStatementGraph->getNode(i)->getResourceGpuHandle(j));
			startRootSlot++;
		}
	}
}

bool ParticleRenderPass::isOpaque()
{
	return _isOpaque;
}

bool ParticleRenderPass::isBounding()
{
	return _isBounding;
}

bool ParticleRenderPass::isWireframe()
{
	return _isWireframe;
}

std::string ParticleRenderPass::getBoundingTextureName()
{
	return _boundingTextureName;
}

void ParticleRenderPass::setBoundingTextureName(std::string textureName)
{
	if (_boundingTextureName == textureName)
		return;

	_boundingTextureName = textureName;
	auto textureManager = TextureManager::getInstance();
	_boundingPolygon = textureManager->getBoundingPolygon(_boundingTextureName, BOUNDING_POLYGON_N);
	setShaderDirty(true);
}

void ParticleRenderPass::setOpaqueness(bool newIsOpaque)
{
	_isOpaque = newIsOpaque;
}

void ParticleRenderPass::setBoundingMode(bool newIsBounding)
{
	if (_isBounding == newIsBounding)
		return;

	_isBounding = newIsBounding;
	updateGeometryShader(_isBounding);
	setShaderDirty(true);
}

void ParticleRenderPass::setWireframe(bool newIsWireframe)
{
	if (_isWireframe == newIsWireframe)
		return;

	_isWireframe = newIsWireframe;
	setShaderDirty(true);
}

void ParticleRenderPass::setVertexShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderVs0 = shader;
	setShaderDirty(true);
}

void ParticleRenderPass::setHullShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderHs0 = shader;
	setShaderDirty(true);
}

void ParticleRenderPass::setDomainShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderDs0 = shader;
	setShaderDirty(true);
}

void ParticleRenderPass::setGeometryShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderGs0 = shader;
	setShaderDirty(true);
}

void ParticleRenderPass::setPixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderPs0 = shader;
	setShaderDirty(true);
}

std::vector<CD3DX12_ROOT_PARAMETER> ParticleRenderPass::buildRootParameter()
{
	// root parameter for renderbase
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(ROOT_SLOT_SIZE);
	slotRootParameter[ROOT_SLOT_OBJECT_CONSTANTS_BUFFER].InitAsConstants(sizeof(ObjectConstants) / 4, 0);

	_passCbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	slotRootParameter[ROOT_SLOT_PASS_CONSTANTS_BUFFER].InitAsDescriptorTable(1, &_passCbvTable);

	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER].InitAsUnorderedAccessView(0); // particles

	_counterTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER].InitAsDescriptorTable(1, &_counterTable);

	slotRootParameter[ROOT_SLOT_BOUNDING_CONSTANTS_BUFFER].InitAsConstants(16, 2);

	buildComputeIndirectRootSignature();

	return slotRootParameter;
}

int ParticleRenderPass::getNumSrvUsing()
{
	return 0;
}

int ParticleRenderPass::getNumUavUsing()
{
	return 3;
}

bool ParticleRenderPass::needsStaticSampler()
{
	return true;
}

ID3D12RootSignature* ParticleRenderPass::getIndirectCommandComputeRootSignature() const
{
	return _computeIndirectRootSignature.Get();
}

ID3D12CommandSignature* ParticleRenderPass::getCommandSignature() const
{
	return _commandSignature.Get();
}

ID3DBlob* ParticleRenderPass::getVertexShader() const
{
	return _shaderVs0.Get();
}

ID3DBlob* ParticleRenderPass::getHullShader() const
{
	return _shaderHs0.Get();
}

ID3DBlob* ParticleRenderPass::getDomainShader() const
{
	return _shaderDs0.Get();
}

ID3DBlob* ParticleRenderPass::getGeometryShader() const
{
	return _shaderGs0.Get();
}

ID3DBlob* ParticleRenderPass::getPixelShader() const
{
	return _shaderPs0.Get();
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& ParticleRenderPass::getInputLayout()
{
	return _inputLayout;
}

void ParticleRenderPass::computeIndirectCommand(
	ID3D12GraphicsCommandList* cmdList, 
	ID3D12PipelineState* computePso,
	UINT numDispatchBlocks)
{
	cmdList->SetComputeRootSignature(getIndirectCommandComputeRootSignature());
	cmdList->SetPipelineState(computePso);

	cmdList->SetComputeRootDescriptorTable(
		0,
		_resource->getCountersUavGpuHandle());
	cmdList->SetComputeRootDescriptorTable(
		1,
		_resource->getIndirectCommandsUavGpuHandle());

	D3D12_RESOURCE_BARRIER toCopy =
		CD3DX12_RESOURCE_BARRIER::Transition(
			_resource->getIndirectCommandsResource(),
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
			D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->ResourceBarrier(1, &toCopy);

	cmdList->CopyBufferRegion(
		_resource->getIndirectCommandsResource(),
		_resource->getCommandBufferCounterOffset(),
		_resource->getIndirectCommandsCounterResetResource(),
		0,
		sizeof(UINT));

	D3D12_RESOURCE_BARRIER toUav =
		CD3DX12_RESOURCE_BARRIER::Transition(
			_resource->getIndirectCommandsResource(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cmdList->ResourceBarrier(1, &toUav);

	cmdList->Dispatch(numDispatchBlocks, 1, 1);
}

// for a assertion
bool textureManagerContains(std::string textureName)
{
	auto textureManager = TextureManager::getInstance();
	for (auto containingTextureName : textureManager->getTextureNames())
	{
		if (containingTextureName == textureName)
			return true;
	}
	
	return false;
}

void ParticleRenderPass::executeIndirectCommand(
	ID3D12GraphicsCommandList* cmdList,
	ID3D12PipelineState* renderPso,
	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology,
	const ObjectConstants& objectConstants,
	const PassConstantBuffer& passCb)
{
	cmdList->SetGraphicsRootSignature(getRootSignature());

	auto vertexBuffers = getEmptyGeometry()->VertexBufferView();
	auto indexBuffer = getEmptyGeometry()->IndexBufferView();

	cmdList->IASetVertexBuffers(0, 1, &vertexBuffers);
	cmdList->IASetIndexBuffer(&indexBuffer);

	cmdList->IASetPrimitiveTopology(primitiveTopology);

	cmdList->SetPipelineState(renderPso);
	cmdList->SetGraphicsRoot32BitConstants(ROOT_SLOT_OBJECT_CONSTANTS_BUFFER, sizeof(ObjectConstants) / 4, &objectConstants, 0);
	cmdList->SetGraphicsRootDescriptorTable(
		ROOT_SLOT_PASS_CONSTANTS_BUFFER, passCb.getGpuHandle());
	cmdList->SetGraphicsRootUnorderedAccessView(
		ROOT_SLOT_PARTICLES_BUFFER, _resource->getCurrentParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootDescriptorTable(
		ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersUavGpuHandle());

	if (_isBounding)
	{
		assert(textureManagerContains(_boundingTextureName) && "isBounding is true but boundingTextureName is invalid.");

		cmdList->SetGraphicsRoot32BitConstants(
			ROOT_SLOT_BOUNDING_CONSTANTS_BUFFER, 16, _boundingPolygon.data(), 0);
	}

	bindGraphicsResourcesOfRegisteredNodes(cmdList, ROOT_SLOT_SIZE);

	_resource->transitCommandBufferToIndirectArgument(cmdList);

	cmdList->ExecuteIndirect(
		_commandSignature.Get(),
		_resource->getEstimatedCurrentNumAliveParticles(),
		_resource->getIndirectCommandsResource(),
		0,
		_resource->getIndirectCommandsResource(),
		_resource->getCommandBufferCounterOffset());
}

MeshGeometry* ParticleRenderPass::getEmptyGeometry()
{
	return _emptyGeometry.get();
}

void ParticleRenderPass::buildComputeIndirectRootSignature()
{
	auto device = DxDevice::getInstance().getD3dDevice();

	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	CD3DX12_DESCRIPTOR_RANGE counterTable;
	counterTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	slotRootParameter[0]
		.InitAsDescriptorTable(1, &counterTable);

	CD3DX12_DESCRIPTOR_RANGE uavTable;
	uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);

	slotRootParameter[1]
		.InitAsDescriptorTable(1, &uavTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		_countof(slotRootParameter),
		slotRootParameter,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(),
		errorBlob.GetAddressOf()
	);

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&_computeIndirectRootSignature)));
}

void ParticleRenderPass::buildCommandSignature()
{
	D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[1] = {};
	argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
	commandSignatureDesc.pArgumentDescs = argumentDescs;
	commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
	commandSignatureDesc.ByteStride = sizeof(ParticleIndirectCommand);

	auto device = DxDevice::getInstance().getD3dDevice();

	ThrowIfFailed(device->CreateCommandSignature(
		&commandSignatureDesc, nullptr, IID_PPV_ARGS(&_commandSignature)));
}

void ParticleRenderPass::generateEmptyGeometry()
{
	auto& device = DxDevice::getInstance();

	using VertexType = DirectX::XMFLOAT3;
	using IndexType = std::uint32_t;

	std::vector<VertexType> vertices(1);

	std::vector<IndexType> indices(4);
	for (int i = 0; i < indices.size(); ++i)
	{
		indices[i] = i;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexType);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(IndexType);

	_emptyGeometry = std::make_unique<MeshGeometry>();
	_emptyGeometry->Name = "emptyGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &_emptyGeometry->VertexBufferCPU));
	CopyMemory(_emptyGeometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &_emptyGeometry->IndexBufferCPU));
	CopyMemory(_emptyGeometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	_emptyGeometry->VertexBufferGPU = DxUtil::createDefaultBuffer(
		device.getD3dDevice().Get(),
		device.getCommandList().Get(),
		vertices.data(),
		vbByteSize,
		_emptyGeometry->VertexBufferUploader
	);

	_emptyGeometry->IndexBufferGPU = DxUtil::createDefaultBuffer(
		device.getD3dDevice().Get(),
		device.getCommandList().Get(),
		indices.data(),
		ibByteSize,
		_emptyGeometry->IndexBufferUploader
	);

	_emptyGeometry->VertexByteStride = sizeof(VertexType);
	_emptyGeometry->VertexBufferByteSize = vbByteSize;
	_emptyGeometry->IndexFormat = DXGI_FORMAT_R32_UINT;
	_emptyGeometry->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	_emptyGeometry->DrawArgs["empty"] = submesh;
}