#include "Core/HlslGeneratorEmit.h"

#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"

HlslGeneratorEmit::HlslGeneratorEmit(std::wstring baseShaderPath)
	: HlslGenerator(baseShaderPath)
{
}

HlslGeneratorEmit::~HlslGeneratorEmit() = default;

void HlslGeneratorEmit::setInitialPosition(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Position");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialVelocity(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Velocity");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialAcceleration(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Acceleration");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialLifetime(UINT float1Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Lifetime");
	newNode->setInput(_nodes[float1Index]);
	addNode(newNode);
	linkNode(float1Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialSize(UINT float1Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Size");
	newNode->setInput(_nodes[float1Index]);
	addNode(newNode);
	linkNode(float1Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialOpacity(UINT float1Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Opacity");
	newNode->setInput(_nodes[float1Index]);
	addNode(newNode);
	linkNode(float1Index, nodeIndex);
}
