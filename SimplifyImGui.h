#pragma once
#include <string>
#include <imgui.h>

namespace GUI
{
    using namespace std;

    // begin - end
    void Begin(const string& windowName, const ImVec2& firstSize = { 100,100 });
    inline void End(void) { ImGui::End(); }
    void ChildFrameBegin(const string& windowName, const ImVec2& size = { 100,100 });
    inline void ChildFrameEnd(void) { ImGui::EndChildFrame(); }

    // return bool
    bool ButtonTrg(const string& buttonName, const ImVec2& size = { 0,0 });
    bool DropDownTrg(const string& dropdownName, int* currentNum, const char* const* items, int arraySize);


    inline void CheckBox(const string& checkboxName, bool* v) { ImGui::Checkbox(checkboxName.c_str(), v); }
}
