#pragma once

#include "Ui/UiNode.h"
#include "Ui/UiLink.h"

#include <string>
#include <vector>

class NodeEditorIo
{
	using Snapshot = std::tuple<std::vector<UiNode>, std::vector<UiLink>, int>;

public:
	static void save(
		const std::vector<UiNode> nodes, 
		const std::vector<UiLink> links, 
		const int currentId,
		const std::wstring filePathWithoutExtension);
	
	static Snapshot load(std::wstring filePath);
};