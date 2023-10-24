#pragma once

#include "Ui/UiNode.h"
#include "Ui/UiLink.h"

#include <vector>
#include <string>


class NodeEditor
{
public:
    NodeEditor();
    virtual ~NodeEditor() = default;

    void show();
    
protected:
    virtual void onCompileButtonClicked() = 0;

    void nextCurrentId(UiNode createdNode);

protected:
    int _currentId;
    std::vector<UiNode> _nodes;
    std::vector<UiLink> _links;
    std::string _name;
};