#include "SimplifyImGui.h"

void GUI::Begin(const string& windowName, const ImVec2& firstSize)
{
    ImGui::SetNextWindowSize(firstSize, ImGuiCond_FirstUseEver);
    ImGui::Begin(windowName.c_str());
}

void GUI::ChildFrameBegin(const string& windowName, const ImVec2& size)
{
    ImGui::BeginChildFrame(ImGui::GetID(windowName.c_str()), size);
}

bool GUI::ButtonTrg(const string& buttonName, const ImVec2& size)
{
    return ImGui::Button(buttonName.c_str(),size);
}

bool GUI::DropDownTrg(const string& dropdownName, int* currentNum, const char* const* items, int arraySize)
{
    return ImGui::Combo(dropdownName.c_str(), currentNum, items, arraySize);
}
