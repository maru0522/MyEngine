#include "SimplifyImGui.h"

void Gui::Begin(const string& windowName, const ImVec2& size)
{
    ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
    ImGui::Begin(windowName.c_str());
}

void Gui::ChildFrameBegin(const string& windowName, const ImVec2& size)
{
    ImGui::BeginChildFrame(ImGui::GetID(windowName.c_str()), size);
}

bool Gui::ButtonTrg(const string& buttonName, const ImVec2& size)
{
    return ImGui::Button(buttonName.c_str(),size);
}

bool Gui::DropDownTrg(const string& dropdownName, int* currentNum, const char* const* items, int arraySize)
{
    return ImGui::Combo(dropdownName.c_str(), currentNum, items, arraySize);
}
