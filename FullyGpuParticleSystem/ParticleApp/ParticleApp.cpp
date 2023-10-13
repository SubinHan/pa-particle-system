#include "ParticleApp.h"

#include "Core/UploadBuffer.h"
#include "ObjectConstantBuffer.h"
#include "Model/Geometry.h"

using namespace DirectX;

PCWSTR ParticleApp::getClassName() const
{
	return L"Particle App Window Class";
}

bool ParticleApp::initialize()
{
	if (!MainWindow::initialize())
		return false;

	auto commandList = _device->startRecordingCommands();

	buildCbvSrvUavDescriptors();
	buildRootSignature();
	buildShadersAndInputLayout();
	buildBoxGeometry();
	buildPso();

	_device->submitCommands(commandList);

	return true;
}

void ParticleApp::OnResize()
{
	MainWindow::OnResize();

	XMMATRIX p = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&_matrixProjection, p);
}

void ParticleApp::update(const GameTimer& gt)
{
	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT4X4 matrixWorld = MathHelper::identity4x4();
	DirectX::XMFLOAT4X4 matrixView = MathHelper::identity4x4();

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&matrixView, view);

	XMMATRIX world = XMLoadFloat4x4(&matrixWorld);
	XMMATRIX proj = XMLoadFloat4x4(&_matrixProjection);
	XMMATRIX worldViewProj = world * view * proj;

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	_objectConstantBuffer->CopyData(0, objConstants);
}

void ParticleApp::draw(const GameTimer& gt)
{
	auto commandList = _device->startRecordingCommands();
	auto currentBackBuffer = _device->getCurrentBackBuffer();
	auto currentBackBufferView = _device->getCurrentBackBufferViewHandle();
	auto depthStencilView = _device->getDepthStencilViewHandle();
	auto cbvSrvUavDescriptorHeap = _device->GetCbvSrvUavDescriptorHeap();

	commandList->RSSetViewports(1, &_device->getScreenViewport());
	commandList->RSSetScissorRects(1, &_device->getScissorRect());

	const auto presentToRender = CD3DX12_RESOURCE_BARRIER::Transition(
		currentBackBuffer,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &presentToRender);


	commandList->ClearRenderTargetView(currentBackBufferView,
		Colors::LightSteelBlue, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencilView,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(1, &currentBackBufferView,
		true, &depthStencilView);

	ID3D12DescriptorHeap* descriptorHeaps[] = { cbvSrvUavDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetGraphicsRootSignature(_rootSignature.Get());

	auto vertexBuffers = _boxGeometry->VertexBufferView();
	auto indexBuffer = _boxGeometry->IndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vertexBuffers);
	commandList->IASetIndexBuffer(&indexBuffer);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootDescriptorTable(
		0, cbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->SetPipelineState(
		_pso.Get());

	commandList->DrawIndexedInstanced(
		_boxGeometry->DrawArgs["box"].IndexCount,
		1, 
		0,
		0, 
		0);

	auto barrierDraw = CD3DX12_RESOURCE_BARRIER::Transition(
		currentBackBuffer,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	commandList->ResourceBarrier(1, &barrierDraw);

	_device->submitCommands(commandList);
	_device->swapBuffers();
}

void ParticleApp::buildCbvSrvUavDescriptors()
{
	// register srv demander

	// register cbv demander
	_objectConstantBuffer =
		std::make_shared<ObjectConstantBuffer>(
			_device->getD3dDevice());

	std::shared_ptr<ICbvSrvUavDemander> basePointer = 
		std::static_pointer_cast<ICbvSrvUavDemander>(_objectConstantBuffer);
	
	_device->registerCbvSrvUavDescriptorDemander(
		std::weak_ptr<ICbvSrvUavDemander>(basePointer));

	// create descriptor heap
	_device->buildCbvSrvUavDescriptorHeap();
}

void ParticleApp::buildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		1,
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

	ThrowIfFailed(_device->getD3dDevice()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&_rootSignature))
	);
}

void ParticleApp::buildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	_vsByteCode = DxUtil::compileShader(L"ParticleApp\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	_psByteCode = DxUtil::compileShader(L"ParticleApp\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void ParticleApp::buildBoxGeometry()
{
	using namespace DirectX;

	std::array<ParticleAppVertex, 8> vertices =
	{
		ParticleAppVertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)}),
		ParticleAppVertex({XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black)}),
		ParticleAppVertex({XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red)}),
		ParticleAppVertex({XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green)}),
		ParticleAppVertex({XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue)}),
		ParticleAppVertex({XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow)}),
		ParticleAppVertex({XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan)}),
		ParticleAppVertex({XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta)})
	};

	std::array<std::uint16_t, 36> indices =
	{
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		4, 5, 1,
		4, 1, 0,

		3, 2, 6,
		3, 6, 7,

		1, 5, 6,
		1, 6, 2,

		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(ParticleAppVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	_boxGeometry = std::make_unique<MeshGeometry>();
	_boxGeometry->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &_boxGeometry->VertexBufferCPU));
	CopyMemory(_boxGeometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &_boxGeometry->IndexBufferCPU));
	CopyMemory(_boxGeometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	_boxGeometry->VertexBufferGPU = DxUtil::createDefaultBuffer(
		_device->getD3dDevice().Get(),
		_device->getCommandList().Get(),
		vertices.data(),
		vbByteSize,
		_boxGeometry->VertexBufferUploader
	);

	_boxGeometry->IndexBufferGPU = DxUtil::createDefaultBuffer(
		_device->getD3dDevice().Get(),
		_device->getCommandList().Get(),
		indices.data(),
		ibByteSize,
		_boxGeometry->IndexBufferUploader
	);

	_boxGeometry->VertexByteStride = sizeof(ParticleAppVertex);
	_boxGeometry->VertexBufferByteSize = vbByteSize;
	_boxGeometry->IndexFormat = DXGI_FORMAT_R16_UINT;
	_boxGeometry->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	_boxGeometry->DrawArgs["box"] = submesh;
}

void ParticleApp::buildPso()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { _inputLayout.data(), static_cast<UINT>(_inputLayout.size()) };
	psoDesc.pRootSignature = _rootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(_vsByteCode->GetBufferPointer()),
		_vsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(_psByteCode->GetBufferPointer()),
		_psByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = _device->getBackBufferFormat();
	bool msaaState = _device->getMsaaState();
	psoDesc.SampleDesc.Count = msaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = msaaState ? _device->getMsaaQuality() - 1 : 0;
	psoDesc.DSVFormat = _device->getDepthStencilFormat();
	ThrowIfFailed(_device->getD3dDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pso)));
}

void ParticleApp::OnMouseLeftDown(int x, int y, short keyState)
{
	lastMousePos.x = x;
	lastMousePos.y = y;

	SetCapture(_hwnd);
}

void ParticleApp::OnMouseLeftUp(int x, int y, short keyState)
{
	ReleaseCapture();
}

void ParticleApp::OnMouseMove(int x, int y, short keyState)
{
	if ((keyState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25 * static_cast<float>(x - lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - lastMousePos.y));

		theta += dx;
		phi += dy;

		phi = MathHelper::clamp(phi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((keyState & MK_RBUTTON) != 0)
	{
		float dx = 0.005f * static_cast<float>(x - lastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - lastMousePos.y);

		radius += dx - dy;

		radius = MathHelper::clamp(radius, 3.0f, 15.0f);
	}

	lastMousePos.x = x;
	lastMousePos.y = y;
}
