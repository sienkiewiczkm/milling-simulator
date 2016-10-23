#include "ImGuiToolboxWindow.hpp"

namespace fw
{

ImGuiToolboxWindow::ImGuiToolboxWindow()
{
}

ImGuiToolboxWindow::~ImGuiToolboxWindow()
{
}

bool ImGuiToolboxWindow::isVisible()
{
    return _isVisible;
}

void ImGuiToolboxWindow::setVisibility(bool isVisible)
{
    _isVisible = isVisible;
}

bool *ImGuiToolboxWindow::getVisibilityFlagPointer()
{
    return &_isVisible;
}

}
