#include "Core/HlslGeneratorEmit.h"

#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"
#include "Core/ShaderStatementGraph.h"

HlslGeneratorEmit::HlslGeneratorEmit(std::wstring baseShaderPath)
	: HlslGenerator(baseShaderPath)
{
}

HlslGeneratorEmit::~HlslGeneratorEmit() = default;

void HlslGeneratorEmit::setInitialPosition(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Position");
	const UINT nodeIndex = _graph->addNode(newNode);;
	newNode->setInput(_graph->getNode(float3Index));
	_graph->linkNode(float3Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialVelocity(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Velocity");
	const UINT nodeIndex = _graph->addNode(newNode);;
	newNode->setInput(_graph->getNode(float3Index));
	_graph->linkNode(float3Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialAcceleration(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.Acceleration");
	const UINT nodeIndex = _graph->addNode(newNode);;
	newNode->setInput(_graph->getNode(float3Index));
	_graph->linkNode(float3Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialLifetime(UINT float1Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.InitialLifetime");
	const UINT nodeIndex = _graph->addNode(newNode);;
	newNode->setInput(_graph->getNode(float1Index));
	
	_graph->linkNode(float1Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialSize(UINT float1Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.InitialSize");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float1Index));
	
	_graph->linkNode(float1Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialOpacity(UINT float1Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("initialColor.w");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float1Index));
	_graph->linkNode(float1Index, nodeIndex);
}

void HlslGeneratorEmit::setInitialColor(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("initialColor.xyz");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float3Index));
	
	_graph->linkNode(float3Index, nodeIndex);
}

void HlslGeneratorEmit::setEndSize(UINT float1Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("newParticle.EndSize");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float1Index));
	
	_graph->linkNode(float1Index, nodeIndex);
}

void HlslGeneratorEmit::setEndOpacity(UINT float1Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("endColor.w");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float1Index));
	_graph->linkNode(float1Index, nodeIndex);
}

void HlslGeneratorEmit::setEndColor(UINT float3Index)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("endColor.xyz");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float3Index));
	
	_graph->linkNode(float3Index, nodeIndex);
}
