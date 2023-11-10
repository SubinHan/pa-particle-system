#include "Core/HlslGeneratorSimulate.h"

#include "Core/ShaderStatementGraph.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloat3ByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloatByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodePointAttractionForce.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeDragForce.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeVortexForce.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeCurlNoiseForce.h"

HlslGeneratorSimulate::HlslGeneratorSimulate(std::wstring baseShaderPath) :
	HlslGenerator(baseShaderPath)
{
}

HlslGeneratorSimulate::~HlslGeneratorSimulate() = default;

UINT HlslGeneratorSimulate::getPosition()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Position");
	const UINT nodeIndex = _graph->addNode(newNode);


	return nodeIndex;
}

UINT HlslGeneratorSimulate::getVelocity()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Velocity");
	const UINT nodeIndex = _graph->addNode(newNode);


	return nodeIndex;
}

UINT HlslGeneratorSimulate::getAcceleration()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Acceleration");
	const UINT nodeIndex = _graph->addNode(newNode);


	return nodeIndex;
}

UINT HlslGeneratorSimulate::pointAttraction(UINT prerequisite, float x, float y, float z, float radius, float strength)
{
	auto newNode =
		std::make_shared<ShaderStatementNodePointAttractionForce>(x, y, z, radius, strength);
	const UINT nodeIndex = _graph->addNode(newNode);

	_graph->linkNode(prerequisite, nodeIndex);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::vortex(UINT prerequisite, float vortexCenterX, float vortexCenterY, float vortexCenterZ, float vortexAxisX, float vortexAxisY, float vortexAxisZ, float magnitude, float tightness)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeVortexForce>(
			vortexCenterX,
			vortexCenterY,
			vortexCenterZ,
			vortexAxisX,
			vortexAxisY,
			vortexAxisZ,
			magnitude,
			tightness);
	const UINT nodeIndex = _graph->addNode(newNode);

	_graph->linkNode(prerequisite, nodeIndex);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::curlNoise(UINT prerequisite, float amplitude, float frequency)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeCurlNoiseForce>(
			newLocalVariableName,
			amplitude,
			frequency);
	const UINT nodeIndex = _graph->addNode(newNode);

	_graph->linkNode(prerequisite, nodeIndex);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::drag(UINT prerequisite, float dragCoefficient)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeDragForce>(dragCoefficient);
	const UINT nodeIndex = _graph->addNode(newNode);

	_graph->linkNode(prerequisite, nodeIndex);

	return nodeIndex;
}

void HlslGeneratorSimulate::setPosition(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Position");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float3Index));

	_graph->linkNode(float3Index, nodeIndex);
}

void HlslGeneratorSimulate::setVelocity(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Velocity");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float3Index));

	_graph->linkNode(float3Index, nodeIndex);
}

void HlslGeneratorSimulate::setAcceleration(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Acceleration");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float3Index));

	_graph->linkNode(float3Index, nodeIndex);
}