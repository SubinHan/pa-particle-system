#include "Core/ParticleRibbonRenderer.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/HlslGeneratorRender.h"
#include "Core/ParticlePreDistanceCalculator.h"
#include "Core/ParticleDistanceCalculator.h"
#include "Model/Geometry.h"
#include "Model/RendererType.h"
#include "Model/RibbonTextureUvType.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/";
static const std::string PIXEL_SHADER_ENTRY_NAME = "RibbonParticlePS";

std::unique_ptr<ParticleRibbonRenderer> ParticleRibbonRenderer::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleRibbonRenderer>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleRibbonRenderer::ParticleRibbonRenderer(ParticleResource* resource, std::string name) :
	ParticleRenderPass(resource, name),
	_preDistanceCalculator(ParticlePreDistanceCalculator::create(resource, name + "preDistanceCalculator")),
	_distanceCalculator(ParticleDistanceCalculator::create(resource, name + "distanceCalculator")),
	_currentRibbonTextureUvType(RibbonTextureUvType::Size)
{
	buildShaders();
	setShaders();
	buildComputePsos();
}

ParticleRibbonRenderer::~ParticleRibbonRenderer() = default;

std::string ParticleRibbonRenderer::getPixelShaderEntryName() const
{
	return PIXEL_SHADER_ENTRY_NAME;
}

void ParticleRibbonRenderer::render(
	ID3D12GraphicsCommandList* cmdList,
	const ObjectConstants& objectConstants, 
	const PassConstantBuffer& passCb)
{
	if (isShaderDirty())
	{
		buildRootSignature();
		rebuildGraphicsPsos();
		setShaderDirty(false);
	}

	if (_currentRibbonTextureUvType == RibbonTextureUvType::DistanceBased)
	{
		_preDistanceCalculator->preRibbonDistance(cmdList);
		_distanceCalculator->calculateRibbonDistanceFromStart(cmdList);
	}

	//computeIndirectCommand(
	//	cmdList, 
	//	_computeIndirectCommandPso.Get(),
		//static_cast<UINT>(ceil(static_cast<float>(_resource->getMaxNumParticles()) / 256.0f)));
	computeIndirectCommand(
		cmdList, 
		_computeIndirectCommandPso.Get(),
		1);
	executeIndirectCommand(
		cmdList, 
		isOpaque()? _ribbonOpaquePso.Get() : _ribbonTranslucentPso.Get(),
		D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
		objectConstants,
		passCb);
}

void ParticleRibbonRenderer::setRibbonTextureUvType(RibbonTextureUvType type)
{
	if (_currentRibbonTextureUvType == type)
		return;

	_currentRibbonTextureUvType = type;

	switch (_currentRibbonTextureUvType)
	{
	case RibbonTextureUvType::SegmentBased:
	{
		setHullShader(_hullShaderSegmentBased);
		break;
	}

	case RibbonTextureUvType::Stretched:
	{
		setHullShader(_hullShaderStretched);
		break;
	}

	case RibbonTextureUvType::DistanceBased:
	{
		setHullShader(_hullShaderDistanceBased);
		break;
	}

	default:
	{
		assert(0 && "Unknown RibbonTextureUvType was given.");
	}
	}
}

void ParticleRibbonRenderer::buildShaders()
{
	const std::wstring graphicsShaderPath = SHADER_ROOT_PATH + L"ParticleRibbon.hlsl";
	const std::wstring indirectCommandShaderPath = SHADER_ROOT_PATH + L"ParticleComputeIndirectCommands.hlsl";

	_vertexShader = DxUtil::compileShader(
		graphicsShaderPath,
		nullptr,
		"RibbonParticleVS",
		"vs_5_1");

	_domainShader= DxUtil::compileShader(
		graphicsShaderPath,
		nullptr,
		"RibbonParticleDS",
		"ds_5_1");

	_hullShaderSegmentBased = DxUtil::compileShader(
		graphicsShaderPath,
		nullptr,
		"RibbonParticleHS_SegmentBased",
		"hs_5_1");

	_hullShaderStretched = DxUtil::compileShader(
		graphicsShaderPath,
		nullptr,
		"RibbonParticleHS_Stretched",
		"hs_5_1");

	_hullShaderDistanceBased = DxUtil::compileShader(
		graphicsShaderPath,
		nullptr,
		"RibbonParticleHS_DistanceBased",
		"hs_5_1");

	_indirectCommandShader = DxUtil::compileShader(
		indirectCommandShaderPath,
		nullptr,
		"RibbonComputeIndirectCommandsCS",
		"cs_5_1");
}

void ParticleRibbonRenderer::rebuildGraphicsPsos()
{
	auto& device = DxDevice::getInstance();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC ribbonOpaquePsoDesc;
	ZeroMemory(&ribbonOpaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	ribbonOpaquePsoDesc.InputLayout = { getInputLayout().data(), static_cast<UINT>(getInputLayout().size()) };
	ribbonOpaquePsoDesc.pRootSignature = getRootSignature();
	ribbonOpaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	ribbonOpaquePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ribbonOpaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	ribbonOpaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	ribbonOpaquePsoDesc.SampleMask = UINT_MAX;
	ribbonOpaquePsoDesc.NumRenderTargets = 1;
	ribbonOpaquePsoDesc.RTVFormats[0] = device.getBackBufferFormat();
	bool msaaState = device.getMsaaState();
	ribbonOpaquePsoDesc.SampleDesc.Count = msaaState ? 4 : 1;
	ribbonOpaquePsoDesc.SampleDesc.Quality = msaaState ? device.getMsaaQuality() - 1 : 0;
	ribbonOpaquePsoDesc.DSVFormat = device.getDepthStencilFormat();
	ribbonOpaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(getVertexShader()->GetBufferPointer()),
		getVertexShader()->GetBufferSize()
	};
	ribbonOpaquePsoDesc.HS =
	{
		reinterpret_cast<BYTE*>(getHullShader()->GetBufferPointer()),
		getHullShader()->GetBufferSize()
	};
	ribbonOpaquePsoDesc.DS =
	{
		reinterpret_cast<BYTE*>(getDomainShader()->GetBufferPointer()),
		getDomainShader()->GetBufferSize()
	};
	ribbonOpaquePsoDesc.GS =
	{
		nullptr,
		0
	};
	ribbonOpaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(getPixelShader()->GetBufferPointer()),
		getPixelShader()->GetBufferSize()
	};
	ribbonOpaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

	ThrowIfFailed(
		device.getD3dDevice()->CreateGraphicsPipelineState(
			&ribbonOpaquePsoDesc, IID_PPV_ARGS(&_ribbonOpaquePso)
		)
	);

	auto ribbonTranslucentPsoDesc = ribbonOpaquePsoDesc;

	ribbonTranslucentPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC translucencyBlendDesc;
	translucencyBlendDesc.BlendEnable = true;
	translucencyBlendDesc.LogicOpEnable = false;
	translucencyBlendDesc.SrcBlend = D3D12_BLEND_ONE;
	translucencyBlendDesc.DestBlend = D3D12_BLEND_ONE;
	translucencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	translucencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_INV_DEST_ALPHA;
	translucencyBlendDesc.DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
	translucencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	translucencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	translucencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	ribbonTranslucentPsoDesc.BlendState.RenderTarget[0] = translucencyBlendDesc;
	ThrowIfFailed(
		device.getD3dDevice()->CreateGraphicsPipelineState(
			&ribbonTranslucentPsoDesc, IID_PPV_ARGS(&_ribbonTranslucentPso)
		)
	);
}

void ParticleRibbonRenderer::setShaders()
{
	setVertexShader(_vertexShader);
	setRibbonTextureUvType(RibbonTextureUvType::DistanceBased);
	setDomainShader(_domainShader);
	setPixelShader(HlslGeneratorRender::generateDefaultPixelShader(PIXEL_SHADER_ENTRY_NAME));
}

void ParticleRibbonRenderer::buildComputePsos()
{
	auto& device = DxDevice::getInstance();

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = getIndirectCommandComputeRootSignature();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_indirectCommandShader->GetBufferPointer()),
		_indirectCommandShader->GetBufferSize()
	};
	ThrowIfFailed(
		device.getD3dDevice()->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_computeIndirectCommandPso)));
}
