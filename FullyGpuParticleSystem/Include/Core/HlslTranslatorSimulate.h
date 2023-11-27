#pragma once

#include "Core/HlslTranslator.h"

class HlslTranslatorSimulate : public HlslTranslator
{
public:
	HlslTranslatorSimulate(
		std::vector<UiNode> nodes, 
		std::vector<UiLink> links);
	virtual ~HlslTranslatorSimulate();

protected:
	virtual bool translateNode(UiNode node) override;
};