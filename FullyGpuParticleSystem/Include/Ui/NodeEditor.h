#pragma once

#include "Ui/UiNode.h"
#include "Ui/UiLink.h"
#include "Ui/IWindow.h"

#include <vector>
#include <string>

class NodeEditor : public IWindow
{
public:
    NodeEditor();
    virtual ~NodeEditor() = default;

    virtual void show() override;
    virtual bool isAlive() override;

    void save();
    virtual void load();

protected:
    virtual std::string getName() = 0;
    virtual void onCompileButtonClicked() = 0;

    void nextCurrentId(UiNode createdNode);

protected:
    int _currentId;
    std::vector<UiNode> _nodes;
    std::vector<UiLink> _links;

    bool _isAlive;
};