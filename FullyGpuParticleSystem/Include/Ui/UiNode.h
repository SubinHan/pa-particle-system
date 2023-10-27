#pragma once

#include <vector>
#include <string>

enum class NodeType;

class UiNode
{
    friend class NodeEditorIo;

public:
    UiNode() = default;
    UiNode(
        const int id,
        std::string nodeName,
        std::vector<std::string> inputNames,
        std::vector<std::string> constantInputNames,
        std::vector<std::string> outputNames,
        NodeType nodeType);

    NodeType getType() const;
    int getId() const;
    std::string getNodeName() const;
    int getNumInputs() const;
    int getNumConstantInputs() const;
    int getNumOutputs() const;
    std::string getInputName(const int index) const;
    std::string getConstantInputName(const int index) const;
    std::string getOutputName(const int index) const;

    bool containsAttributeAsInput(const int attributeId) const;
    bool containsAttributeAsOutput(const int attributeId) const;
    int getInputIndexByAttributeId(const int attributeId) const;
    int getInputId(const int index) const;
    int getOutputId();

    float* getConstantInputAddress(const int index);
    float getConstantInputValue(const int index) const;

private:
    int _id;
    std::string _nodeName;
    std::vector<std::string> _inputNames;
    std::vector<std::string> _constantInputNames;
    std::vector<std::string> _outputNames;
    NodeType _nodeType;

    std::vector<float> _constantInputValues;
};