#include "Core/ShaderStatementNode/ShaderStatementNodeCurlNoiseForce.h"

#include "Core/DxDevice.h"
#include "Util/MathHelper.h"
#include "Util/DxDebug.h"

#include <d3dx12.h>
#include <assert.h>

constexpr int TEXTURE_SIZE = 128;
constexpr DXGI_FORMAT TEXTURE_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;

ShaderStatementNodeCurlNoiseForce::ShaderStatementNodeCurlNoiseForce(std::string variableName, float amplitude, float frequency) :
	ShaderStatementNode(variableName),
	_amplitude(amplitude),
	_frequency(frequency),
	_noiseTextureVariableName(""),
	_srvCreated(false)
{
	buildResource();
	DxDevice& device = DxDevice::getInstance();
	device.registerCbvSrvUavDescriptorDemander(this);
}

ShaderStatementNodeCurlNoiseForce::~ShaderStatementNodeCurlNoiseForce()
{
	DxDevice& device = DxDevice::getInstance();
	device.unregisterCbvSrvUavDescriptorDemander(this);
}

int ShaderStatementNodeCurlNoiseForce::getNumDescriptorsToDemand() const
{
	return 1;
}

void ShaderStatementNodeCurlNoiseForce::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	DxDevice& device = DxDevice::getInstance();

	_hCpu = hCpu;
	_hGpu = hGpu;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = _noiseTextureResource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = _noiseTextureResource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	device.getD3dDevice()->CreateShaderResourceView(
		_noiseTextureResource.Get(), &srvDesc, _hCpu);

	_srvCreated = true;
}

std::string ShaderStatementNodeCurlNoiseForce::generateStatements() const
{
	std::string sampleTexture =
		"float3 " + _variableName + " = " + _noiseTextureVariableName + ".SampleLevel(gsamLinearWrap, currentPosition * " + std::to_string(_frequency) + ", 0); \n";
	std::string updateVelocity = "currentVelocity += " + _variableName + ";";

	return sampleTexture + updateVelocity;
}

int ShaderStatementNodeCurlNoiseForce::getNumResourcesToBind() const
{
	return 1;
}

ResourceViewType ShaderStatementNodeCurlNoiseForce::getResourceViewType(int index) const
{
	assert(index == 0 && "index is wrong");
	return ResourceViewType::Srv;
}

std::string ShaderStatementNodeCurlNoiseForce::getTypeInShader(int index) const
{
	assert(index == 0 && "index is wrong");
	return "Texture3D";
}

void ShaderStatementNodeCurlNoiseForce::onResourceBound(std::vector<std::string> variableNamesInShader)
{
	assert(variableNamesInShader.size() == 1 && "wrong request submitted.");
	_noiseTextureVariableName = variableNamesInShader[0];
}

bool ShaderStatementNodeCurlNoiseForce::isResourceViewCreated(int index) const
{
	assert(index == 0 && "index is wrong");
	return _srvCreated;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ShaderStatementNodeCurlNoiseForce::getResourceGpuHandle(int index) const
{
	assert(index == 0 && "index is wrong");
	return _hGpu;
}

void ShaderStatementNodeCurlNoiseForce::buildResource()
{
	DxDevice& device = DxDevice::getInstance();
	auto d3dDevice = device.getD3dDevice();
	auto commandList = device.startRecordingCommands();

	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	texDesc.Alignment = 0;
	texDesc.Width = TEXTURE_SIZE;
	texDesc.Height = TEXTURE_SIZE;
	texDesc.DepthOrArraySize = TEXTURE_SIZE;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_noiseTextureResource)));

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBufferResource;
	const UINT numSubresources = texDesc.MipLevels;
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(
		_noiseTextureResource.Get(),
		0,
		numSubresources);

	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto uploadBuffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadBuffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_uploadBufferResource)));

	DirectX::XMFLOAT3* noiseData =
		new DirectX::XMFLOAT3[TEXTURE_SIZE * TEXTURE_SIZE * TEXTURE_SIZE];
	int sizeSquare = TEXTURE_SIZE * TEXTURE_SIZE;

	for (int i = 0; i < TEXTURE_SIZE; ++i)
	{
		for (int j = 0; j < TEXTURE_SIZE; ++j)
		{
			for (int k = 0; k < TEXTURE_SIZE; ++k)
			{
				int index = i * sizeSquare + j * TEXTURE_SIZE + k;

				float x = (MathHelper::randF() - 0.5f) * _amplitude;
				float y = (MathHelper::randF() - 0.5f) * _amplitude;
				float z = (MathHelper::randF() - 0.5f) * _amplitude;

				noiseData[index] = DirectX::XMFLOAT3(x, y, z);
			}
		}
	}

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = noiseData;
	subResourceData.RowPitch = sizeSquare * sizeof(DirectX::XMFLOAT3);
	subResourceData.SlicePitch = TEXTURE_SIZE;

	auto readToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
		_noiseTextureResource.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		D3D12_RESOURCE_STATE_COPY_DEST);

	auto copyDestToRead = CD3DX12_RESOURCE_BARRIER::Transition(
		_noiseTextureResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);

	commandList->ResourceBarrier(1, &readToCopyDest);
	UpdateSubresources(commandList.Get(), _noiseTextureResource.Get(), _uploadBufferResource.Get(),
		0, 0, numSubresources, &subResourceData);
	commandList->ResourceBarrier(1, &copyDestToRead);

	device.submitCommands(commandList);
	delete[] noiseData;
}


