#pragma once

#include <string>
#include <vector>
#include <wrl.h>

struct ID3DBlob;

class HlslTranslator
{
public:
	HlslTranslator(std::wstring baseShaderPath);
	~HlslTranslator();

	UINT createNewLocalRandFloat3();

	Microsoft::WRL::ComPtr<ID3DBlob> compile();

private:
	std::vector<std::string> _localVariableNames;
};