#pragma once

#include "Ui/UiNode.h"

enum class NodeType;

class UiNodeFactory
{
public:
	static UiNode createNode(const int id, NodeType nodeType);
	static UiNode createEmpty(const int id);
	static UiNode createNewFloat(const int id);
	static UiNode createNewFloat3(const int id);
	static UiNode createNewFloat4(const int id);
	static UiNode createRandFloat3(const int id);
	static UiNode createAddFloat3(const int id);
	static UiNode createEmitterOutput(const int id);
};