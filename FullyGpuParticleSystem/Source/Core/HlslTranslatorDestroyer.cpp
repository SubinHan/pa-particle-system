#include "Core/HlslTranslatorDestroyer.h"

#include "Core/HlslGeneratorSimulate.h"

static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleDestroyCS.hlsl";

HlslTranslatorDestroyer::HlslTranslatorDestroyer(std::vector<UiNode> nodes, std::vector<UiLink> links) :
	HlslTranslatorSimulate(nodes, links)
{
}

HlslTranslatorDestroyer::~HlslTranslatorDestroyer() = default;

std::unique_ptr<HlslGenerator> HlslTranslatorDestroyer::createHlslGenerator()
{
	return std::make_unique<HlslGeneratorSimulate>(BASE_SIMULATOR_SHADER_PATH);
}

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslatorDestroyer::compileShaderImpl(std::wstring shaderPath)
{
	return DxUtil::compileShader(
		shaderPath,
		nullptr,
		"DestroyCS",
		"cs_5_1");
}