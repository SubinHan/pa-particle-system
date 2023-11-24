#include "Core/ParticleSpriteRenderer.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/HlslGeneratorRender.h"
#include "Model/Geometry.h"
#include "Model/RendererType.h"
#include "Model/RibbonTextureUvType.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/";
static const std::string PIXEL_SHADER_ENTRY_NAME = "ParticlePS";
static const std::wstring SHADER_PATH = SHADER_ROOT_PATH + L"ParticleSprite.hlsl";
static const std::wstring INDIRECT_COMMAND_SHADER_PATH = SHADER_ROOT_PATH + L"ParticleComputeIndirectCommands.hlsl";

std::unique_ptr<ParticleSpriteRenderer> ParticleSpriteRenderer::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleSpriteRenderer>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleSpriteRenderer::ParticleSpriteRenderer(ParticleResource* resource, std::string name) :
	ParticleRenderPass(resource, name),
	_currentRibbonTextureUvType(RibbonTextureUvType::SegmentBased)
{
	initShaders();
	buildComputePsos();
}

std::string ParticleSpriteRenderer::getPixelShaderEntryName() const
{
	return PIXEL_SHADER_ENTRY_NAME;
}

void ParticleSpriteRenderer::render(
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

	computeIndirectCommand(cmdList, _computeIndirectCommandPso.Get(), 1);
	executeIndirectCommand(
		cmdList,
		isOpaque()? _spriteOpaquePso.Get() : _spriteTranslucentPso.Get(), 
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		objectConstants, 
		passCb);
}

void ParticleSpriteRenderer::updateGeometryShader(bool isBounding)
{
	static constexpr D3D_SHADER_MACRO defines[] =
	{
		"BOUNDING",
		"1",
		NULL,
		NULL,
	};

	setGeometryShader(DxUtil::compileShader(
		SHADER_PATH,
		isBounding ? defines : nullptr,
		"SpriteParticleGS",
		"gs_5_1"));
}

void ParticleSpriteRenderer::initShaders()
{
	_indirectCommandShader = DxUtil::compileShader(
		INDIRECT_COMMAND_SHADER_PATH,
		nullptr,
		"ComputeIndirectCommandsCS",
		"cs_5_1");

	setVertexShader(DxUtil::compileShader(
		SHADER_PATH,
		nullptr,
		"SpriteParticleVS",
		"vs_5_1"));
	updateGeometryShader(isBounding());
	setPixelShader(HlslGeneratorRender::generateDefaultPixelShader(PIXEL_SHADER_ENTRY_NAME));
}

void ParticleSpriteRenderer::rebuildGraphicsPsos()
{
	auto& device = DxDevice::getInstance();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC spriteOpaquePsoDesc;
	ZeroMemory(&spriteOpaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	spriteOpaquePsoDesc.InputLayout = { getInputLayout().data(), static_cast<UINT>(getInputLayout().size()) };
	spriteOpaquePsoDesc.pRootSignature = getRootSignature();
	spriteOpaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	spriteOpaquePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	spriteOpaquePsoDesc.RasterizerState.FillMode = isWireframe() ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	spriteOpaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	spriteOpaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	spriteOpaquePsoDesc.SampleMask = UINT_MAX;
	spriteOpaquePsoDesc.NumRenderTargets = 1;
	spriteOpaquePsoDesc.RTVFormats[0] = device.getBackBufferFormat();
	bool msaaState = device.getMsaaState();
	spriteOpaquePsoDesc.SampleDesc.Count = msaaState ? 4 : 1;
	spriteOpaquePsoDesc.SampleDesc.Quality = msaaState ? device.getMsaaQuality() - 1 : 0;
	spriteOpaquePsoDesc.DSVFormat = device.getDepthStencilFormat();
	spriteOpaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(getVertexShader()->GetBufferPointer()),
		getVertexShader()->GetBufferSize()
	};
	spriteOpaquePsoDesc.HS =
	{
		nullptr,
		0
	};
	spriteOpaquePsoDesc.DS =
	{
		nullptr,
		0
	};
	spriteOpaquePsoDesc.GS =
	{
		reinterpret_cast<BYTE*>(getGeometryShader()->GetBufferPointer()),
		getGeometryShader()->GetBufferSize()
	};
	spriteOpaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(getPixelShader()->GetBufferPointer()),
		getPixelShader()->GetBufferSize()
	};
	spriteOpaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	ThrowIfFailed(
		device.getD3dDevice()->CreateGraphicsPipelineState(
			&spriteOpaquePsoDesc, IID_PPV_ARGS(&_spriteOpaquePso)
		)
	);

	auto spriteTranslucentPsoDesc = spriteOpaquePsoDesc;

	spriteTranslucentPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

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

	spriteTranslucentPsoDesc.BlendState.RenderTarget[0] = translucencyBlendDesc;
	ThrowIfFailed(
		device.getD3dDevice()->CreateGraphicsPipelineState(
			&spriteTranslucentPsoDesc, IID_PPV_ARGS(&_spriteTranslucentPso)
		)
	);
}

void ParticleSpriteRenderer::buildComputePsos()
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
