#pragma once
#include <string>
#include <imgui.h>

namespace GUI
{
    // begin - end
    void Begin(const std::string& windowName, const ImVec2& firstSize = { 100,100 });
    inline void End(void) { ImGui::End(); }
    void ChildFrameBegin(const std::string& windowName, const ImVec2& size = { 100,100 });
    inline void ChildFrameEnd(void) { ImGui::EndChildFrame(); }

    // return bool
    bool ButtonTrg(const std::string& buttonName, const ImVec2& size = { 0,0 });
    bool DropDownTrg(const std::string& dropdownName, int* currentNum, const char* const* items, int arraySize);


    inline void CheckBox(const std::string& checkboxName, bool* v) { ImGui::Checkbox(checkboxName.c_str(), v); }
}
