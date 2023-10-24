#include "Core/HlslGeneratorSimulate.h"

#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloat3ByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloatByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"

HlslGeneratorSimulate::HlslGeneratorSimulate(std::wstring baseShaderPath) :
	HlslGenerator(baseShaderPath)
{
}

HlslGeneratorSimulate::~HlslGeneratorSimulate() = default;

UINT HlslGeneratorSimulate::getPositionAfterSimulation()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Position");
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::getVelcotiyAfterSimulateAcceleration()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Velocity");
	addNode(newNode);

	return nodeIndex;
}

void HlslGeneratorSimulate::setPosition(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Position");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslGeneratorSimulate::setNextFrameVelocity(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Velocity");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslGeneratorSimulate::setNextFrameAcceleration(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Acceleration");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}