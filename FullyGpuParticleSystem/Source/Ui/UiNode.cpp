#include "Ui/UiNode.h"

UiNode::UiNode(
	const int id,
	std::string nodeName,
	std::vector<std::string> inputNames,
	std::vector<std::string> constantInputNames,
	std::vector<std::string> outputNames,
	NodeType nodeType) :
	_id(id),
	_nodeName(nodeName),
	_inputNames(inputNames),
	_constantInputNames(constantInputNames),
	_constantInputValues(constantInputNames.size()),
	_outputNames(outputNames),
	_nodeType(nodeType)
{
}

NodeType UiNode::getType() const
{
	return _nodeType;
}

int UiNode::getId() const
{
	return _id;
}

std::string UiNode::getNodeName() const
{
	return _nodeName;
}

int UiNode::getNumInputs() const
{
	return _inputNames.size();
}

int UiNode::getNumConstantInputs() const
{
	return _constantInputNames.size();
}

int UiNode::getNumOutputs() const
{
	return _outputNames.size();
}

std::string UiNode::getInputName(const int index) const
{
	return _inputNames[index];
}

std::string UiNode::getConstantInputName(const int index) const
{
	return _constantInputNames[index];
}

std::string UiNode::getOutputName(const int index) const
{
	return _outputNames[index];
}

bool UiNode::containsAttributeAsInput(const int attributeId) const
{
	return _id < attributeId && attributeId <= _id + getNumInputs();
}

int UiNode::getInputIndexByAttributeId(const int attributeId) const
{
	return attributeId - _id - 1;
}

int UiNode::getInputId(const int index) const
{
	return _id + index + 1;
}

int UiNode::getOutputId()
{
	return _id + getNumInputs() + getNumConstantInputs() + 1;
}

float* UiNode::getConstantInputAddress(const int index)
{
	return &_constantInputValues[index];
}

float UiNode::getConstantInput(const int index)
{
	return _constantInputValues[index];
}
