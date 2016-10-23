#pragma once

namespace fw
{

class ImGuiToolboxWindow
{
public:
    ImGuiToolboxWindow();
    virtual ~ImGuiToolboxWindow();

    bool isVisible();
    void setVisibility(bool isVisible);
    bool *getVisibilityFlagPointer();

    virtual void update() = 0;

protected:
    bool _isVisible;
};

}
