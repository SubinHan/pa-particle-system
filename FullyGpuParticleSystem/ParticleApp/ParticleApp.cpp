#include "ParticleApp.h"

#include "Core/DxDevice.h"
#include "Core/ImguiInitializer.h"
#include "Core/UploadBuffer.h"
#include "Core/PassConstantBuffer.h"
#include "Core/ParticleSystem.h"
#include "Core/ParticleSystemManager.h"
#include "Core/TextureBuffer.h"
#include "Core/TextureManager.h"
#include "Core/MaterialManager.h"
#include "Model/Geometry.h"
#include "Model/Material.h"
#include "Model/Texture.h"
#include "Util/DDSTextureLoader.h"
#include "Ui/NodeEditorEmit.h"
#include "Ui/ParticleSystemController.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeCurlNoiseForce.h"

#include <d3d12.h>
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include <chrono>
#include <thread>

using namespace DirectX;

ParticleApp::ParticleApp(HINSTANCE hInstance) :
	MainWindow(hInstance)
{
	// do not call initialize(). because:
	// will be initialized through the callback function.
}

ParticleApp::~ParticleApp()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

PCWSTR ParticleApp::getClassName() const
{
	return L"Particle App Window Class";
}

bool ParticleApp::initialize()
{
	if (!MainWindow::initialize())
		return false;

	auto commandList = _device->startRecordingCommands();

	_imguiInitializer = std::make_unique<ImguiInitializer>(_device, _hwnd);

	_particleSystemManager = std::make_unique<ParticleSystemManager>(_device);

	_particleSystemController =
		std::make_unique<ParticleSystemController>(_particleSystemManager.get());

	_passConstantBuffer =
		std::make_shared<PassConstantBuffer>(
			_device->getD3dDevice());

	// be careful to order!! load texture - buildDescriptors - buildMaterials
	loadTextures(commandList.Get());
	registerCbvSrvUavDescriptors();
	buildMaterials();
	buildRootSignature();
	buildShadersAndInputLayout();
	buildPso();

	_device->submitCommands(commandList);

	return true;
}

void ParticleApp::onResize()
{
	MainWindow::onResize();

	XMMATRIX p = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&_proj, p);
}

void ParticleApp::update(const GameTimer& gt)
{
	updateCamera(gt);

	//auto& device = DxDevice::getInstance();

	//if (device.getCurrentFence() != 0 && device.getFence()->GetCompletedValue() < device.getCurrentFence())
	//{
	//	HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
	//	ThrowIfFailed(device.getFence()->SetEventOnCompletion(_device->getCurrentFence(), eventHandle));
	//	WaitForSingleObject(eventHandle, INFINITE);
	//	CloseHandle(eventHandle);
	//}

	XMMATRIX view = XMLoadFloat4x4(&this->_view);
	XMMATRIX proj = XMLoadFloat4x4(&this->_proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	auto viewDeterminant = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&viewDeterminant, view);
	auto projDeterminant = XMMatrixDeterminant(proj);
	XMMATRIX invProj = XMMatrixInverse(&projDeterminant, proj);
	auto viewProjDeterminant = XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = XMMatrixInverse(&viewProjDeterminant, viewProj);

	XMStoreFloat4x4(&_passConstants.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&_passConstants.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&_passConstants.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&_passConstants.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&_passConstants.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&_passConstants.InvViewProj, XMMatrixTranspose(invViewProj));
	_passConstants.EyePosW = _eyePos;

	auto clientWidth = _device->getClientWidth();
	auto clientHeight = _device->getClientHeight();

	_passConstants.RenderTargetSize = XMFLOAT2((float)clientWidth, (float)clientHeight);
	_passConstants.InvRenderTargetSize = XMFLOAT2(1.0f / clientWidth, 1.0f / clientHeight);
	_passConstants.NearZ = 1.0f;
	_passConstants.FarZ = 1000.0f;
	_passConstants.TotalTime = gt.totalTime();
	_passConstants.DeltaTime = gt.deltaTime();

	_passConstantBuffer->copyData(0, _passConstants);
}

void ParticleApp::draw(const GameTimer& gt)
{
	drawObjects(gt);
	drawUi();

	auto commandList = _device->startRecordingCommands();

	auto currentBackBuffer = _device->getCurrentBackBuffer();
	auto barrierDraw = CD3DX12_RESOURCE_BARRIER::Transition(
		currentBackBuffer,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	commandList->ResourceBarrier(1, &barrierDraw);

	_device->submitCommands(commandList);
    _device->swapBuffers();
}

void ParticleApp::drawObjects(const GameTimer& gt)
{
	auto commandList = _device->startRecordingCommands();

	auto cbvSrvUavDescriptorHeap = _device->getCbvSrvUavDescriptorHeap();

	ID3D12DescriptorHeap* descriptorHeaps[] = { cbvSrvUavDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	auto currentBackBuffer = _device->getCurrentBackBuffer();
	auto currentBackBufferView = _device->getCurrentBackBufferViewHandle();
	auto depthStencilView = _device->getDepthStencilViewHandle();

	commandList->RSSetViewports(1, &_device->getScreenViewport());
	commandList->RSSetScissorRects(1, &_device->getScissorRect());

	const auto presentToRender = CD3DX12_RESOURCE_BARRIER::Transition(
		currentBackBuffer,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &presentToRender);

	commandList->ClearRenderTargetView(currentBackBufferView,
		Colors::Black, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencilView,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(1, &currentBackBufferView,
		true, &depthStencilView);

	fireDrawToParticleSystems(commandList.Get(), gt);
	_device->submitCommands(commandList);
}

void ParticleApp::drawUi()
{
	auto commandList = _device->startRecordingCommands();

	auto cbvSrvUavDescriptorHeap = _device->getCbvSrvUavDescriptorHeap();

	ID3D12DescriptorHeap* descriptorHeaps[] = { cbvSrvUavDescriptorHeap.Get() };

	auto currentBackBuffer = _device->getCurrentBackBuffer();
	auto currentBackBufferView = _device->getCurrentBackBufferViewHandle();
	auto depthStencilView = _device->getDepthStencilViewHandle();

	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	commandList->RSSetViewports(1, &_device->getScreenViewport());
	commandList->RSSetScissorRects(1, &_device->getScissorRect());
	commandList->OMSetRenderTargets(1, &currentBackBufferView,
		true, &depthStencilView);


	// render imgui

	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	bool windowOpen = true;
	ImGui::ShowDemoWindow(&windowOpen);
	_particleSystemController->show();

	// Rendering
	ImGui::Render();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
	_device->submitCommands(commandList);
}

void ParticleApp::loadTextures(ID3D12GraphicsCommandList* commandList)
{
	// parallel vectors:
	const std::vector<std::string> texturesName = {
		"default",
		"fire",
		"uv_mapper"
	};

	const std::vector<std::wstring> texturesPath = {
		L"textures/circle_05.dds",
		L"textures/fire_subuv.dds",
		L"textures/uv_mapper.dds",
	};

	assert(texturesName.size() == texturesPath.size());

	TextureManager* textureManager = TextureManager::getInstance();

	for (int i = 0; i < texturesName.size(); ++i)
	{
		textureManager->loadTexture(
			commandList,
			_device,
			texturesName[i],
			texturesPath[i]);
	}
}
void ParticleApp::registerCbvSrvUavDescriptors()
{
	// register cbv demander

	_device->registerCbvSrvUavDescriptorDemander(
		_passConstantBuffer.get());

	// register imgui
	_device->registerCbvSrvUavDescriptorDemander(
		_imguiInitializer.get());
}

void ParticleApp::buildMaterials()
{
	MaterialManager* materialManager = MaterialManager::getInstance();

	Material defaultMat{};

	defaultMat.Name = "default";
	defaultMat.MatCBIndex = 0;
	defaultMat.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	defaultMat.FresnelR0 = { 0.3f, 0.3f, 0.3f };
	defaultMat.DiffuseTextureName = "default";
	defaultMat.Roughness = 0.01f;

	//_materials[particleMat->Name] = std::move(particleMat);

	materialManager->addMaterial(defaultMat);
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

void ParticleApp::updateCamera(const GameTimer& gt)
{
	// Convert Spherical to Cartesian coordinates.
	_eyePos.x = radius * sinf(phi) * cosf(theta);
	_eyePos.z = radius * sinf(phi) * sinf(theta);
	_eyePos.y = radius * cosf(phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(_eyePos.x, _eyePos.y, _eyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&this->_view, view);
}

void ParticleApp::fireDrawToParticleSystems(ID3D12GraphicsCommandList* cmdList, const GameTimer& gt)
{
	for (int i = 0; i < _particleSystemManager->getNumParticleSystems(); ++i)
	{
		_particleSystemManager->getParticleSystemByIndex(i)->onDraw(cmdList, *_passConstantBuffer, gt);
	}
}

void ParticleApp::onMouseLeftDown(int x, int y, short keyState)
{
	lastMousePos.x = x;
	lastMousePos.y = y;

	SetCapture(_hwnd);
}

void ParticleApp::onMouseLeftUp(int x, int y, short keyState)
{
	ReleaseCapture();
}

void ParticleApp::onMouseMove(int x, int y, short keyState)
{
	if ((keyState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - lastMousePos.x));
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

		radius = MathHelper::clamp(radius, 1.0f, 15.0f);
	}

	lastMousePos.x = x;
	lastMousePos.y = y;
}
