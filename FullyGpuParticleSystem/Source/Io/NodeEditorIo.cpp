#include "Io/NodeEditorIo.h"

#include "Ui/ValueType.h"

#include <imnodes.h>

#include <fstream>

decltype(auto) readString(std::fstream& fin, int stringSize)
{
    char buffer[256];

    fin.read(
        reinterpret_cast<char*>(buffer),
        static_cast<std::streamsize>(stringSize));

    return std::string(buffer, buffer + stringSize);
}

decltype(auto) readSizeType(std::fstream& fin)
{
    size_t variable;

    fin.read(
        reinterpret_cast<char*>(&variable),
        static_cast<std::streamsize>(sizeof(decltype(variable))));

    return variable;
}


decltype(auto) readInt(std::fstream& fin)
{
    int variable;

    fin.read(
        reinterpret_cast<char*>(&variable),
        static_cast<std::streamsize>(sizeof(decltype(variable))));

    return variable;
}

decltype(auto) readFloat(std::fstream& fin)
{
    float variable;

    fin.read(
        reinterpret_cast<char*>(&variable),
        static_cast<std::streamsize>(sizeof(decltype(variable))));

    return variable;
}

void NodeEditorIo::save(
    const std::vector<UiNode> nodes,
    const std::vector<UiLink> links, 
    const int currentId,
    const std::wstring filePathWithoutExtension)
{
    std::string filePathString;
    filePathString.assign(filePathWithoutExtension.begin(), filePathWithoutExtension.end());
    std::string iniPath = filePathString + ".ini";
    std::string bytePath = filePathString + ".byte";

    // Save the internal imnodes state
    ImNodes::SaveCurrentEditorStateToIniFile(iniPath.c_str());

    // Dump our editor state as bytes into a file

    std::fstream fout(
        bytePath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

    // copy the node vector to file
    const size_t numNodes = nodes.size();
    fout.write(
        reinterpret_cast<const char*>(&numNodes),
        static_cast<std::streamsize>(sizeof(size_t)));

    for (int i = 0; i < numNodes; ++i)
    {
        auto id = nodes[i].getId();
        fout.write(
            reinterpret_cast<const char*>(&id),
            static_cast<std::streamsize>(sizeof(int)));

        auto nodeNameSize = nodes[i].getNodeName().size();
        fout.write(
            reinterpret_cast<const char*>(&nodeNameSize),
            static_cast<std::streamsize>(sizeof(size_t)));

        auto nodeName = nodes[i].getNodeName();
        fout.write(
            reinterpret_cast<const char*>(nodeName.data()),
            static_cast<std::streamsize>(nodeName.size()));

        auto numInputs = nodes[i].getNumInputs();
        fout.write(
            reinterpret_cast<const char*>(&numInputs),
            static_cast<std::streamsize>(sizeof(decltype(numInputs))));
        for (int j = 0; j < numInputs; ++j)
        {
            auto inputNameSize = nodes[i].getInputName(j).size();
            fout.write(
                reinterpret_cast<const char*>(&inputNameSize),
                static_cast<std::streamsize>(sizeof(decltype(inputNameSize))));

            auto inputName = nodes[i].getInputName(j);
            fout.write(
                reinterpret_cast<const char*>(inputName.data()),
                static_cast<std::streamsize>(inputName.size()));
        }

        auto numConstantInputs = nodes[i].getNumConstantInputs();
        fout.write(
            reinterpret_cast<const char*>(&numConstantInputs),
            static_cast<std::streamsize>(sizeof(decltype(numConstantInputs))));

        for (int j = 0; j < numConstantInputs; ++j)
        {
            auto constnatInputSize = nodes[i].getConstantInputName(j).size();
            fout.write(
                reinterpret_cast<const char*>(&constnatInputSize),
                static_cast<std::streamsize>(sizeof(decltype(constnatInputSize))));

            auto constnatInputName = nodes[i].getConstantInputName(j);
            fout.write(
                reinterpret_cast<const char*>(constnatInputName.data()),
                static_cast<std::streamsize>(constnatInputName.size()));

            auto constnatInputType = nodes[i].getConstantInputValueType(j);
            fout.write(
                reinterpret_cast<const char*>(&constnatInputType),
                static_cast<std::streamsize>(sizeof(decltype(constnatInputType))));
        }

        auto numOutputs = nodes[i].getNumOutputs();
        fout.write(
            reinterpret_cast<const char*>(&numOutputs),
            static_cast<std::streamsize>(sizeof(decltype(numOutputs))));

        for (int j = 0; j < numOutputs; ++j)
        {
            auto outputNameSize = nodes[i].getOutputName(j).size();
            fout.write(
                reinterpret_cast<const char*>(&outputNameSize),
                static_cast<std::streamsize>(sizeof(decltype(outputNameSize))));

            auto outputName = nodes[i].getOutputName(j);
            fout.write(
                reinterpret_cast<const char*>(outputName.data()),
                static_cast<std::streamsize>(outputName.size()));
        }

        auto nodeType = nodes[i].getType();
        fout.write(
            reinterpret_cast<const char*>(&nodeType),
            static_cast<std::streamsize>(sizeof(decltype(nodeType))));

        for (int j = 0; j < numConstantInputs; ++j)
        {
            switch (nodes[i].getConstantInputValueType(j))
            {
            case ValueType::Float:
            {
                auto constantInputValue = nodes[i].getConstantInputValue(j);
                fout.write(
                    reinterpret_cast<const char*>(&constantInputValue),
                    static_cast<std::streamsize>(sizeof(decltype(constantInputValue))));
                break;
            }

            case ValueType::String:
            {
                auto constantInputStringSize = nodes[i].getConstantInputValueAsString(j).size();
                fout.write(
                    reinterpret_cast<const char*>(&constantInputStringSize),
                    static_cast<std::streamsize>(sizeof(decltype(constantInputStringSize))));

                auto constantInputString = nodes[i].getConstantInputValueAsString(j);
                fout.write(
                    reinterpret_cast<const char*>(constantInputString.data()),
                    static_cast<std::streamsize>(constantInputString.size()));
                break;
            }
            default:
                assert(0 && "unknown value type");
                break;
            }
        }
    }

    //fout.write(
    //    reinterpret_cast<const char*>(nodes.data()),
    //    static_cast<std::streamsize>(sizeof(UiNode) * numNodes));

    // copy the link vector to file
    const size_t numLinks = links.size();
    fout.write(
        reinterpret_cast<const char*>(&numLinks),
        static_cast<std::streamsize>(sizeof(size_t)));
    fout.write(
        reinterpret_cast<const char*>(links.data()),
        static_cast<std::streamsize>(sizeof(UiLink) * numLinks));

    // copy the current_id to file
    fout.write(
        reinterpret_cast<const char*>(&currentId), static_cast<std::streamsize>(sizeof(int)));
}

NodeEditorIo::Snapshot NodeEditorIo::load(std::wstring filePathWithoutExtension)
{
    std::string filePathString;
    filePathString.assign(filePathWithoutExtension.begin(), filePathWithoutExtension.end());
    std::string iniPath = filePathString + ".ini";
    std::string bytePath = filePathString + ".byte";

    // Load the internal imnodes state
    ImNodes::LoadCurrentEditorStateFromIniFile(iniPath.c_str());

    // Load our editor state into memory

    std::fstream fin(bytePath, std::ios_base::in | std::ios_base::binary);

    if (!fin.is_open())
    {
        return std::make_tuple(std::vector<UiNode>(), std::vector<UiLink>(), 0);
    }

    // copy nodes into memory
    std::vector<UiNode> nodes;

    size_t numNodes;
    fin.read(reinterpret_cast<char*>(&numNodes), static_cast<std::streamsize>(sizeof(size_t)));

    for (int i = 0; i < numNodes; ++i)
    {
        UiNode node;

        node._id = readInt(fin);

        size_t nodeNameSize = readSizeType(fin);

        node._nodeName = readString(fin, nodeNameSize);

        int numInputs = readInt(fin);
        node._inputNames.resize(numInputs);

        for (int j = 0; j < numInputs; ++j)
        {
            size_t inputNameSize = readSizeType(fin);

            node._inputNames[j] = readString(fin, inputNameSize);
        }

        int numConstantInputs = readInt(fin);
        node._constantInputNames.resize(numConstantInputs);
        node._constantInputValues.resize(numConstantInputs);
        node._constantInputValueTypes.resize(numConstantInputs);
        node._constantInputStrings.resize(numConstantInputs);
        node._constantInputStringsCstr.resize(numConstantInputs);

        for (int j = 0; j < numConstantInputs; ++j)
        {
            size_t constantInputNameSize = readSizeType(fin);

            node._constantInputNames[j] = readString(fin, constantInputNameSize);

            ValueType valueType;
            fin.read(
                reinterpret_cast<char*>(&node._constantInputValueTypes[j]),
                static_cast<std::streamsize>(sizeof(decltype(valueType))));
        }

        int numOutputs = readInt(fin);
        node._outputNames.resize(numOutputs);

        for (int j = 0; j < numOutputs; ++j)
        {
            size_t outputNameSize = readSizeType(fin);

            node._outputNames[j] = readString(fin, outputNameSize);
        }

        NodeType nodeType;
        fin.read(
            reinterpret_cast<char*>(&node._nodeType),
            static_cast<std::streamsize>(sizeof(decltype(nodeType))));

        for (int j = 0; j < numConstantInputs; ++j)
        {
            switch (node.getConstantInputValueType(j))
            {
            case ValueType::Float:
            {
                float value = readFloat(fin);
                node._constantInputValues[j] = value;
                break;
            }

            case ValueType::String:
            {
                size_t constantInputStringSize = readSizeType(fin);

                node._constantInputStrings[j] = readString(fin, constantInputStringSize);
                // hack code!!
                node._constantInputStringsCstr[j].assign(node._constantInputStrings[j].begin(), node._constantInputStrings[j].end());
                node._constantInputStringsCstr[j].push_back('\0');
                break;
            }
            default:
                assert(0 && "unknown value type");
                break;
            }
        }

        nodes.push_back(node);
    }

    //fin.read(
    //    reinterpret_cast<char*>(nodes.data()),
    //    static_cast<std::streamsize>(sizeof(UiNode) * numNodes));

    // copy links into memory
    std::vector<UiLink> links;
    
    size_t numLinks = readSizeType(fin);
    links.resize(numLinks);
    fin.read(
        reinterpret_cast<char*>(links.data()),
        static_cast<std::streamsize>(sizeof(UiLink) * numLinks));

    // copy current_id into memory
    int currentId;
    fin.read(reinterpret_cast<char*>(&currentId), static_cast<std::streamsize>(sizeof(int)));
    
    return std::make_tuple(nodes, links, currentId);
}