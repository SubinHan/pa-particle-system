#include "Ui/UiNode.h"

#include "Ui/ValueType.h"

#include <assert.h>

UiNode::UiNode(
	const int id,
	std::string nodeName,
	std::vector<std::string> inputNames,
	std::vector<std::string> constantInputNames,
	std::vector<ValueType> constantValueTypes,
	std::vector<std::string> outputNames,
	NodeType nodeType) :
	_id(id),
	_nodeName(nodeName),
	_inputNames(inputNames),
	_constantInputNames(constantInputNames),
	_constantInputValueTypes(constantValueTypes),
	_constantInputValues(constantInputNames.size()),
	_constantInputStrings(constantInputNames.size()),
	_constantInputStringsCstr(constantInputNames.size()),
	_outputNames(outputNames),
	_nodeType(nodeType)
{
	assert(constantInputNames.size() == constantValueTypes.size() && "constantInputNames's size is not same with constantValueTypes' size");

	for (int i = 0; i < constantInputNames.size(); ++i)
	{
		_constantInputStrings[i].reserve(64);
	}

	for (int i = 0; i < constantInputNames.size(); ++i)
	{
		_constantInputStringsCstr[i].resize(64);
	}
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

ValueType UiNode::getConstantInputValueType(const int index) const
{
	return _constantInputValueTypes[index];
}

std::string UiNode::getOutputName(const int index) const
{
	return _outputNames[index];
}

bool UiNode::containsAttributeAsInput(const int attributeId) const
{
	return _id < attributeId && attributeId <= _id + getNumInputs();
}

bool UiNode::containsAttributeAsOutput(const int attributeId) const
{
	return _id + getNumInputs() + getNumConstantInputs() < attributeId && attributeId <= _id + getNumInputs() + getNumConstantInputs() + getNumOutputs();
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

char* UiNode::getConstantInputStringAddress(const int index)
{
	return _constantInputStringsCstr[index].data();
}

float UiNode::getConstantInputValue(const int index) const
{
	return _constantInputValues[index];
}

std::string UiNode::getConstantInputValueAsString(const int index) const
{
	return _constantInputStrings[index];
}

// little hack code, I should input by using char pointer,
// but modifying inner data of std::string by passing char pointer through
// string.data() function is undefined behavior.
// so I use vector<char> represents string data, and update actual
// std::string data by using this function.
void UiNode::updateConstantInputStringFromCstr(int index)
{
	_constantInputStrings[index] = std::string(_constantInputStringsCstr[index].data());
}
