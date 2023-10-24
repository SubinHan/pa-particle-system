#include "Ui/NodeEditor.h"

#include "Ui/NodeType.h"
#include "Ui/UiNodeFactory.h"

#include "imgui.h"
#include "imnodes.h"

NodeEditor::NodeEditor() :
    _currentId(0),
    _nodes(),
    _links(),
    _name("HelloNodeEditor")
{
}

void NodeEditor::show()
{
    ImGui::Begin(_name.c_str());
    if (ImGui::Button("Compile"))
    {
        onCompileButtonClicked();
    }
    ImGui::TextUnformatted("Text");

    ImNodes::BeginNodeEditor();

    // Context menu (popup)
    {
        NodeType selectedNodeType = NodeType::Size;
        if (ImGui::BeginPopupContextWindow())
        {
            ImGui::Text("Create a new node");

            const int numNodes = static_cast<int>(NodeType::Size);
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
            ImGui::DragFloat(node.getConstantInputName(i).c_str(), node.getConstantInputAddress(i), 0.01f);
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

void NodeEditor::nextCurrentId(UiNode createdNode)
{
    _currentId +=
        createdNode.getNumInputs() +
        createdNode.getNumConstantInputs() +
        createdNode.getNumOutputs() + 1;
}