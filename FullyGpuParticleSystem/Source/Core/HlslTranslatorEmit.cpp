#include "Core/HlslTranslatorEmit.h"

#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"

HlslTranslatorEmit::HlslTranslatorEmit(std::wstring baseShaderPath)
	: HlslTranslator(baseShaderPath)
{
}

HlslTranslatorEmit::~HlslTranslatorEmit() = default;

void HlslTranslatorEmit::setInitialPosition(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Position");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslTranslatorEmit::setInitialVelocity(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Velocity");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslTranslatorEmit::setInitialAcceleration(UINT float3Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Acceleration");
	newNode->setInput(_nodes[float3Index]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
}

void HlslTranslatorEmit::setInitialLifetime(UINT float1Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Lifetime");
	newNode->setInput(_nodes[float1Index]);
	addNode(newNode);
	linkNode(float1Index, nodeIndex);
}

void HlslTranslatorEmit::setInitialSize(UINT float1Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Size");
	newNode->setInput(_nodes[float1Index]);
	addNode(newNode);
	linkNode(float1Index, nodeIndex);
}

void HlslTranslatorEmit::setInitialOpacity(UINT float1Index)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Opacity");
	newNode->setInput(_nodes[float1Index]);
	addNode(newNode);
	linkNode(float1Index, nodeIndex);
}
