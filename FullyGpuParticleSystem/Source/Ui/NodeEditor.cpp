#include "Ui/NodeEditor.h"

#include "Ui/NodeType.h"
#include "Ui/ValueType.h"
#include "Ui/UiNodeFactory.h"
#include "Io/NodeEditorIo.h"

#include "imgui.h"
#include "imnodes.h"

static const std::wstring SAVE_LOAD_ROOT_PATH = L"Saved/";

NodeEditor::NodeEditor() :
    _currentId(0),
    _nodes(),
    _links(),
    _isAlive(true)
{
}

void NodeEditor::show()
{
    ImGui::Begin(getName().c_str(), &_isAlive);
    if (ImGui::Button("Compile"))
    {
        onCompileButtonClicked();
        save();
    }
    ImGui::TextUnformatted("Text");

    ImNodes::BeginNodeEditor();

    // Context menu (popup)
    {
        NodeType selectedNodeType = NodeType::Size;
        if (ImGui::BeginPopupContextWindow())
        {
            ImGui::Text("Create a new node");

            auto [nodeNames, nodeTypes] = getCreatableNodesImpl();
            assert(nodeNames.size() == nodeTypes.size() && "getCreatableNodes() didn't return parallel vectors");

            const int numNodes = nodeNames.size();
            for (int i = 0; i < numNodes; ++i)
            {
                if (ImGui::Selectable(nodeNames[i].c_str()))
                {
                    selectedNodeType = nodeTypes[i];
                }
            }
            ImGui::EndPopup();
        }

        // if selected
        if (selectedNodeType != NodeType::Size)
        {
            const int node_id = _currentId;
            ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
            const auto createdNode = 
                UiNodeFactory::createNode(node_id, selectedNodeType);
            _nodes.push_back(createdNode);
            nextCurrentId(createdNode);
        }
    }

    // show nodes
    for (UiNode& node : _nodes)
    {
        const int nodeId = node.getId();
        const int numInputs = node.getNumInputs();
        const int numConstantInputs = node.getNumConstantInputs();
        const int numOutputs = node.getNumOutputs();
        ImNodes::BeginNode(nodeId);

        int currentId = nodeId;
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node.getNodeName().c_str());
        ImNodes::EndNodeTitleBar();

        for (int i = 0; i < numInputs; ++i)
        {
            currentId++;

            ImNodes::BeginInputAttribute(currentId);
            ImGui::TextUnformatted(node.getInputName(i).c_str());
            ImNodes::EndInputAttribute();
        }

        for (int i = 0; i < numConstantInputs; ++i)
        {
            currentId++;

            ImNodes::BeginStaticAttribute(currentId);
            ImGui::PushItemWidth(120.f);

            const std::string label = node.getConstantInputName(i);
            
            switch (node.getConstantInputValueType(i))
            {
            case ValueType::Float:
            {
                ImGui::DragFloat(node.getConstantInputName(i).c_str(), node.getConstantInputAddress(i), 0.01f);
                break;
            }
            case ValueType::String:
            {
                ImGui::InputText(label.c_str(), node.getConstantInputStringAddress(i), 64);
                node.updateConstantInputStringFromCstr(i);
                break;
            }
            default:
                assert(0 && "unknown value type");
                break;
            }
            
            ImGui::PopItemWidth();
            ImNodes::EndStaticAttribute();
        }

        for (int i = 0; i < numOutputs; ++i)
        {
            currentId++;

            ImNodes::BeginOutputAttribute(currentId);
            const float textWidth = ImGui::CalcTextSize(node.getOutputName(i).c_str()).x;
            ImGui::Indent(120.f - textWidth);
            ImGui::TextUnformatted(node.getOutputName(i).c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }

    for (const UiLink& link : _links)
    {
        ImNodes::Link(link.getId(), link.getFromId(), link.getToId());
    }

    ImNodes::EndNodeEditor();

    // delete node
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImGui::IsKeyReleased(ImGuiKey_Delete))
    {
        int numNodesToDelete = ImNodes::NumSelectedNodes();
        if (numNodesToDelete > 0)
        {
            std::vector<int> nodesToDelete(numNodesToDelete);
            ImNodes::GetSelectedNodes(nodesToDelete.data());

            for (int i = 0; i < numNodesToDelete; ++i)
            {
                deleteNode(nodesToDelete[i]);
            }
        }
    }

    {
        int from;
        int to;
        if (ImNodes::IsLinkCreated(&from, &to))
        {
            UiLink link(_currentId++, from, to);
            _links.push_back(link);
        }
    }

    {
        int link_id;
        if (ImNodes::IsLinkDestroyed(&link_id))
        {
            auto iter =
                std::find_if(_links.begin(), _links.end(), [link_id](const UiLink& link) -> bool {
                return link.getId() == link_id;
                    });
            assert(iter != _links.end());
            _links.erase(iter);
        }
    }

    ImGui::End();
}

bool NodeEditor::isAlive()
{
    return _isAlive;
}

void NodeEditor::save()
{
    std::wstring nameWstring;
    std::string name = getName();
    nameWstring.assign(name.begin(), name.end());
    NodeEditorIo::save(_nodes, _links, _currentId, SAVE_LOAD_ROOT_PATH + nameWstring);
}

void NodeEditor::load()
{
    std::wstring nameWstring;
    std::string name = getName();
    nameWstring.assign(name.begin(), name.end());
    auto [nodes, links, currentId] = 
        NodeEditorIo::load(SAVE_LOAD_ROOT_PATH + nameWstring);

    _nodes = nodes;
    _links = links;
    _currentId = currentId;
}

std::pair<std::vector<std::string>, std::vector<NodeType>> NodeEditor::getCreatableNodesImpl() const
{
    static const std::vector<std::string> creatableNodeNames =
    {
        "NewFloat",
        "NewFloat3",
        "NewFloat4",
        "AddFloat",
        "AddFloat3",
        "MaskX",
        "MaskY",
        "MaskZ",
        "MaskW",
        "MakeFloat3",
        "MakeFloat4",
        "MakeFloat4ByColorAlpha",
        "MultiplyFloat3ByScalar",
        "MultiplyFloat",
        "SetColorOfFloat4",
        "SinTime",
    };

    static const std::vector<NodeType> creatableNodeTypes =
    {
        NodeType::NewFloat,
        NodeType::NewFloat3,
        NodeType::NewFloat4,
        NodeType::AddFloat,
        NodeType::AddFloat3,
        NodeType::MaskX,
        NodeType::MaskY,
        NodeType::MaskZ,
        NodeType::MaskW,
        NodeType::MakeFloat3,
        NodeType::MakeFloat4,
        NodeType::MakeFloat4ByColorAlpha,
        NodeType::MultiplyFloat3ByScalar,
        NodeType::MultiplyFloat,
        NodeType::SetColorOfFloat4,
        NodeType::SinTime,
    };

    auto [childCreatableNodeNames, childCreatableNodeTypes] = getCreatableNodes();

    childCreatableNodeNames.insert(
        childCreatableNodeNames.end(),
        creatableNodeNames.begin(),
        creatableNodeNames.end());

    childCreatableNodeTypes.insert(
        childCreatableNodeTypes.end(),
        creatableNodeTypes.begin(),
        creatableNodeTypes.end());
        
    return std::make_pair<>(childCreatableNodeNames, childCreatableNodeTypes);
}

void NodeEditor::nextCurrentId(UiNode createdNode)
{
    _currentId +=
        createdNode.getNumInputs() +
        createdNode.getNumConstantInputs() +
        createdNode.getNumOutputs() + 1;
}

void NodeEditor::deleteNode(int nodeId)
{
    // find node
    for (int i = 0; i < _nodes.size(); ++i)
    {
        auto& node = _nodes[i];
        if (node.getId() != nodeId)
            continue;

        // delete all related links.
        for (int j = 0; j < _links.size(); ++j)
        {
            auto& link = _links[j];
            if (node.containsAttributeAsInput(link.getFromId()) ||
                node.containsAttributeAsInput(link.getToId()) ||
                node.containsAttributeAsOutput(link.getFromId()) ||
                node.containsAttributeAsOutput(link.getToId()))
            {
                _links.erase(_links.begin() + j);
                j--;
            }
        }

        _nodes.erase(_nodes.begin() + i);
        return;
    }
}