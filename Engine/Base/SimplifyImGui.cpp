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

bool GUI::DropDownTrg(const std::string& arg_label, std::string& arg_currentItem, const std::vector<std::string>& arg_list)
{
    // ドロップダウン内で、選択中のアイテムが変更されたか
    bool is_change = false;
    if (ImGui::BeginCombo(arg_label.c_str(), arg_currentItem.c_str()))
    {
        // 配列を全検索
        for (auto& item : arg_list)
        {
            // 選択中のアイテムとして選んだ名前が、現在の要素と一致しているか
            bool is_select = (arg_currentItem == item);

            // 現在の要素を、ドロップダウンの表示項目として追加
            if (ImGui::Selectable(item.c_str(), is_select))
            {
                // 現在の要素を選択中のアイテムとする
                arg_currentItem = item;
                // 選択中のアイテムが変更された
                is_change = true;
            }

            // 選択中のアイテムとして指定した名前が、現在の要素と一致している場合
            if (is_select)
            {
                // 現在の要素を、ドロップダウンを畳んでいた際に、一番上に表示する。
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return is_change;
}

void GUI::CheckBox(const std::string& checkboxName, bool* v)
{
    ImGui::Checkbox(checkboxName.c_str(), v);
}

void GUI::Text(const char* fmt, ...)
{
    ImGui::Text(fmt);
}

void GUI::BlankLine(void)
{
    ImGui::Spacing();
}

void GUI::List(const std::string& arg_label, const std::vector<std::string>& arg_list)
{
    // タブ
    if (ImGui::CollapsingHeader(arg_label.c_str())) // 開かれてるかの有無
    {
        // リスト全検索
        for (auto& elem : arg_list)
        {
            // テキストの羅列
            ImGui::BulletText("%s", elem.c_str());
        }
    }
}

void GUI::SearchList(const std::string& arg_label, const std::vector<std::string>& arg_list)
{
    if (ImGui::CollapsingHeader(arg_label.c_str()))
    {
        static ImGuiTextFilter filter;
        filter.Draw();

        // リスト全検索
        for (auto& elem : arg_list)
        {
            // 検索中の内容を考慮
            if (filter.PassFilter(elem.c_str()))
            {
                // テキストの羅列
                ImGui::BulletText("%s", elem.c_str());
            }
        }
    }
}

void GUI::HelpMaker(const std::string& arg_helpMsg, const std::string& arg_Mark)
{
    ImGui::TextDisabled(arg_Mark.c_str());
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(arg_helpMsg.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
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
    float* v[3] = { &arg_value.x, &arg_value.y, &arg_value.z };
    ImGui::SliderFloat3(arg_label.c_str(), *v, arg_value_min, arg_value_max);
}

