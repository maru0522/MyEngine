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

void GUI::SliderFloat(const std::string& arg_label, float* arg_value, float arg_value_min, float arg_value_max)
{
    ImGui::SliderFloat(arg_label.c_str(), arg_value, arg_value_min, arg_value_max);
}

void GUI::SliderFloat2(const std::string& arg_label, float arg_value[2], float arg_value_min, float arg_value_max)
{
    ImGui::SliderFloat2(arg_label.c_str(), arg_value, arg_value_min, arg_value_max);
}

void GUI::SliderFloat2(const std::string& arg_label, Vector2& arg_value, float arg_value_min, float arg_value_max)
{
    float* v[2] = { &arg_value.x, &arg_value.y };
    ImGui::SliderFloat2(arg_label.c_str(), *v, arg_value_min, arg_value_max);
}

void GUI::SliderFloat3(const std::string& arg_label, float arg_value[3], float arg_value_min, float arg_value_max)
{
    ImGui::SliderFloat3(arg_label.c_str(), arg_value, arg_value_min, arg_value_max);
}

void GUI::SliderFloat3(const std::string& arg_label, Vector3& arg_value, float arg_value_min, float arg_value_max)
{
    float* v[3] = { &arg_value.x, &arg_value.y, &arg_value.z};
    ImGui::SliderFloat3(arg_label.c_str(), *v, arg_value_min, arg_value_max);
}

