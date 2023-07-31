#include "SimplifyImGui.h"

void GUI::Begin(const std::string& windowName, const ImVec2& firstSize)
{
    ImGui::SetNextWindowSize(firstSize, ImGuiCond_FirstUseEver);
    ImGui::Begin(windowName.c_str());
}

void GUI::End(void)
{
    ImGui::End();
}

void GUI::ChildFrameBegin(const std::string& windowName, const ImVec2& size)
{
    ImGui::BeginChildFrame(ImGui::GetID(windowName.c_str()), size);
}

void GUI::ChildFrameEnd(void)
{
    ImGui::EndChildFrame();
}

bool GUI::ButtonTrg(const std::string& buttonName, const ImVec2& size)
{
    return ImGui::Button(buttonName.c_str(), size);
}

bool GUI::DropDownTrg(const std::string& dropdownName, int* currentNum, const char* const* items, int arraySize)
{
    return ImGui::Combo(dropdownName.c_str(), currentNum, items, arraySize);
}

void GUI::CheckBox(const std::string& checkboxName, bool* v)
{
    ImGui::Checkbox(checkboxName.c_str(), v);
}

void GUI::Text(const char* fmt, ...)
{
    ImGui::Text(fmt);
}

void GUI::Space(void)
{
    ImGui::Spacing();
}
