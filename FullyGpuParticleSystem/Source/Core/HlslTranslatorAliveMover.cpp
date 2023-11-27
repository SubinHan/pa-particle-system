#include "Core/HlslTranslatorAliveMover.h"

#include "Core/HlslGeneratorSimulate.h"

static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleMoveAlivesCS.hlsl";

HlslTranslatorAliveMover::HlslTranslatorAliveMover(std::vector<UiNode> nodes, std::vector<UiLink> links) :
	HlslTranslatorSimulate(nodes, links)
{
}

HlslTranslatorAliveMover::~HlslTranslatorAliveMover() = default;

std::unique_ptr<HlslGenerator> HlslTranslatorAliveMover::createHlslGenerator()
{
	return std::make_unique<HlslGeneratorSimulate>(BASE_SIMULATOR_SHADER_PATH);
}

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslatorAliveMover::compileShaderImpl(std::wstring shaderPath)
{
	return DxUtil::compileShader(
		shaderPath,
		nullptr,
		"MoveAlivesCS",
		"cs_5_1");
}