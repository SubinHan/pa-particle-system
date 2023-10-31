#include "Core/HlslGeneratorSimulate.h"

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
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Position");
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::getVelocity()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Velocity");
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::getAcceleration()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "particles[particleIndex].Acceleration");
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::pointAttraction(UINT prerequisite, float x, float y, float z, float radius, float strength)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodePointAttractionForce>(x, y, z, radius, strength);
	addNode(newNode);
	linkNode(prerequisite, nodeIndex);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::vortex(UINT prerequisite, float vortexCenterX, float vortexCenterY, float vortexCenterZ, float vortexAxisX, float vortexAxisY, float vortexAxisZ, float magnitude, float tightness)
{
	const UINT nodeIndex = _nodes.size();
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
	addNode(newNode);
	linkNode(prerequisite, nodeIndex);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::curlNoise(UINT prerequisite, float amplitude, float frequency)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeCurlNoiseForce>(
			newLocalVariableName,
			amplitude,
			frequency);
	addNode(newNode);
	linkNode(prerequisite, nodeIndex);

	return nodeIndex;
}

UINT HlslGeneratorSimulate::drag(UINT prerequisite, float dragCoefficient)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeDragForce>(dragCoefficient);
	addNode(newNode);
	linkNode(prerequisite, nodeIndex);

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

void HlslGeneratorSimulate::setVelocity(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Velocity");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslGeneratorSimulate::setAcceleration(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("particles[particleIndex].Acceleration");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}