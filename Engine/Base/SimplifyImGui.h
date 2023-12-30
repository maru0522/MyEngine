#pragma once
#include <string>
#include <imgui.h>
#include "Vector2.h"
#include "Vector3.h"
/**
 * @file SimplifyImGui.h
 * @brief ImGuiの関数を自分用に、使いやすく/分かりやすく するためのラッパー関数が宣言されているファイル
 */

namespace GUI
{
    // begin - end
    void Begin(const std::string& windowName, const ImVec2& firstSize = { 100,100 });
    void End(void);
    void ChildFrameBegin(const std::string& windowName, const ImVec2& size = { 100,100 });
    void ChildFrameEnd(void);

    // return bool
    bool ButtonTrg(const std::string& buttonName, const ImVec2& size = { 0,0 });
    bool DropDownTrg(const std::string& dropdownName, int* currentNum, const char* const* items, int arraySize);
    bool DropDownTrg(const std::string& arg_label, std::string& arg_currentItem, const std::vector<std::string>& arg_list);

    void CheckBox(const std::string& checkboxName, bool* v);
    void Text(const char* fmt,...);
    void BlankLine(void);

    void List(const std::string& arg_label, const std::vector<std::string>& arg_list);
    void SearchList(const std::string& arg_label, const std::vector<std::string>& arg_list);

    void HelpMaker(const std::string& arg_helpMsg, const std::string& arg_Mark = "(?)");

    void SliderFloat(const std::string& arg_label, float* arg_value, float arg_value_min, float arg_value_max);
    void SliderFloat2(const std::string& arg_label, float arg_value[2], float arg_value_min, float arg_value_max);
    void SliderFloat2(const std::string& arg_label, Vector2& arg_value, float arg_value_min, float arg_value_max);
    void SliderFloat3(const std::string& arg_label, float arg_value[3], float arg_value_min, float arg_value_max);
    void SliderFloat3(const std::string& arg_label, Vector3& arg_value, float arg_value_min, float arg_value_max);
}
