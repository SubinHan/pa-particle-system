#include "Io/NodeEditorIo.h"

#include <imnodes.h>

#include <fstream>

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
            auto constantInputValue = nodes[i].getConstantInputValue(j);
            fout.write(
                reinterpret_cast<const char*>(&constantInputValue),
                static_cast<std::streamsize>(sizeof(decltype(constantInputValue))));
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

        int id;
        fin.read(
            reinterpret_cast<char*>(&id),
            static_cast<std::streamsize>(sizeof(int)));
        node._id = id;

        size_t nodeNameSize;
        fin.read(
            reinterpret_cast<char*>(&nodeNameSize),
            static_cast<std::streamsize>(sizeof(size_t)));

        std::string nodeName;
        nodeName.resize(nodeNameSize);
        fin.read(
            reinterpret_cast<char*>(nodeName.data()),
            static_cast<std::streamsize>(nodeNameSize));
        node._nodeName = nodeName;

        int numInputs;
        fin.read(
            reinterpret_cast<char*>(&numInputs),
            static_cast<std::streamsize>(sizeof(decltype(numInputs))));
        node._inputNames.resize(numInputs);

        for (int j = 0; j < numInputs; ++j)
        {
            size_t inputNameSize;
            fin.read(
                reinterpret_cast<char*>(&inputNameSize),
                static_cast<std::streamsize>(sizeof(decltype(inputNameSize))));
            node._inputNames[j].resize(inputNameSize);

            fin.read(
                reinterpret_cast<char*>(node._inputNames[j].data()),
                static_cast<std::streamsize>(inputNameSize));
        }

        int numConstantInputs;
        fin.read(
            reinterpret_cast<char*>(&numConstantInputs),
            static_cast<std::streamsize>(sizeof(decltype(numConstantInputs))));
        node._constantInputNames.resize(numConstantInputs);
        node._constantInputValues.resize(numConstantInputs);

        for (int j = 0; j < numConstantInputs; ++j)
        {
            size_t constantInputNameSize;
            fin.read(
                reinterpret_cast<char*>(&constantInputNameSize),
                static_cast<std::streamsize>(sizeof(decltype(constantInputNameSize))));
            node._constantInputNames[j].resize(constantInputNameSize);

            fin.read(
                reinterpret_cast<char*>(node._constantInputNames[j].data()),
                static_cast<std::streamsize>(constantInputNameSize));
        }

        int numOutputs;
        fin.read(
            reinterpret_cast<char*>(&numOutputs),
            static_cast<std::streamsize>(sizeof(decltype(numOutputs))));
        node._outputNames.resize(numOutputs);

        for (int j = 0; j < numOutputs; ++j)
        {
            size_t outputNameSize;
            fin.read(
                reinterpret_cast<char*>(&outputNameSize),
                static_cast<std::streamsize>(sizeof(decltype(outputNameSize))));
            node._outputNames[j].resize(outputNameSize);

            fin.read(
                reinterpret_cast<char*>(node._outputNames[j].data()),
                static_cast<std::streamsize>(outputNameSize));
        }

        NodeType nodeType;
        fin.read(
            reinterpret_cast<char*>(&node._nodeType),
            static_cast<std::streamsize>(sizeof(decltype(nodeType))));

        for (int j = 0; j < numConstantInputs; ++j)
        {
            float constantInputValue;
            fin.read(
                reinterpret_cast<char*>(&node._constantInputValues[j]),
                static_cast<std::streamsize>(sizeof(decltype(constantInputValue))));
        }

        nodes.push_back(node);
    }

    //fin.read(
    //    reinterpret_cast<char*>(nodes.data()),
    //    static_cast<std::streamsize>(sizeof(UiNode) * numNodes));

    // copy links into memory
    std::vector<UiLink> links;
    
    size_t numLinks;
    fin.read(reinterpret_cast<char*>(&numLinks), static_cast<std::streamsize>(sizeof(size_t)));
    links.resize(numLinks);
    fin.read(
        reinterpret_cast<char*>(links.data()),
        static_cast<std::streamsize>(sizeof(UiLink) * numLinks));

    // copy current_id into memory
    int currentId;
    fin.read(reinterpret_cast<char*>(&currentId), static_cast<std::streamsize>(sizeof(int)));
    
    return std::make_tuple(nodes, links, currentId);
}
