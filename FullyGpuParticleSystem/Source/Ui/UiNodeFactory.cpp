#include "Ui/UiNodeFactory.h"

#include "Ui/NodeType.h"

#include <cassert>

UiNode UiNodeFactory::createNode(const int id, NodeType nodeType)
{
	switch (nodeType)
	{
	case NodeType::Empty:
		return createEmpty(id);
	case NodeType::NewFloat:
		return createNewFloat(id);
	case NodeType::NewFloat3:
		return createNewFloat3(id);
	case NodeType::NewFloat4:
		return createNewFloat4(id);
	case NodeType::RandFloat3:
		return createRandFloat3(id);
	case NodeType::AddFloat3:
		return createAddFloat3(id);
	case NodeType::EmitterOutput:
		return createEmitterOutput(id);
	default:
		// should never reach here.
		assert(0);
	}

	return UiNode(
		-1, 
		"error",
		std::vector<std::string>(), 
		std::vector<std::string>(), 
		std::vector<std::string>(), 
		NodeType::Size);
}

UiNode UiNodeFactory::createEmpty(const int id)
{
	std::vector<std::string> emptyVector;

	return UiNode(id, "Empty", emptyVector, emptyVector, emptyVector, NodeType::Empty);
}

UiNode UiNodeFactory::createNewFloat(const int id)
{
	constexpr auto nodeType = NodeType::NewFloat;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantInputNames =
	{
		"r",
	};
	std::vector<std::string> outputNames =
	{
		"float"
	};
	std::string nodeName = nodeNames[static_cast<int>(nodeType)];

	return UiNode(id, nodeName, emptyVector, constantInputNames, outputNames, nodeType);
}

UiNode UiNodeFactory::createNewFloat3(const int id)
{
	constexpr auto nodeType = NodeType::NewFloat3;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantInputNames =
	{
		"r",
		"g",
		"b",
	};
	std::vector<std::string> outputNames =
	{
		"float3"
	};
	std::string nodeName = nodeNames[static_cast<int>(nodeType)];

	return UiNode(id, nodeName, emptyVector, constantInputNames, outputNames, nodeType);
}

UiNode UiNodeFactory::createNewFloat4(const int id)
{
	constexpr auto nodeType = NodeType::NewFloat4;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantInputNames =
	{
		"r",
		"g",
		"b",
		"a",
	};
	std::vector<std::string> outputNames =
	{
		"float4"
	};
	std::string nodeName = nodeNames[static_cast<int>(nodeType)];

	return UiNode(id, nodeName, emptyVector, constantInputNames, outputNames, nodeType);
}

UiNode UiNodeFactory::createRandFloat3(const int id)
{
	constexpr auto nodeType = NodeType::RandFloat3;
	std::vector<std::string> emptyVector;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, outputNames, nodeType);
}

UiNode UiNodeFactory::createAddFloat3(const int id)
{
	constexpr auto nodeType = NodeType::AddFloat3;
	std::vector<std::string> inputNames = 
	{
		"float3input0",
		"float3input1",
	};
	std::vector<std::string> emptyVector;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, outputNames, nodeType);
}

UiNode UiNodeFactory::createEmitterOutput(const int id)
{
	constexpr auto nodeType = NodeType::EmitterOutput;
	std::vector<std::string> inputNames =
	{
		"initialPosition",
		"initialVelocity",
		"initialAcceleration",
		"initialLifetime",
		"initialSize",
		"initialOpacity",
	};
	std::vector<std::string> emptyVector;

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyVector, nodeType);
}
