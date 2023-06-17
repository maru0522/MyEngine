#include "LightGroup.h"
#include <imgui.h>

LightGroup::LightGroup(void)
{
    DefaultLightSetting();
    cbLightGroup_.Create();
    TransferCB();
}

void LightGroup::Update(void)
{
    // 値の更新があった時だけ定数バッファに転送する
    if (isDirty_) {
        TransferCB();
        isDirty_ = false;
    }
    //ImGui::ShowDemoWindow();
    DebugImGui();
}

void LightGroup::Draw(void)
{
    InitDirectX::GetInstance()->GetCommandList()->
        SetGraphicsRootConstantBufferView(4, cbLightGroup_.GetBuffer()->GetGPUVirtualAddress());
}

void LightGroup::TransferCB(void)
{
    cbLightGroup_.GetConstBuffMap()->ambientColor_ = ambientColor_;

    for (size_t i = 0; i < kDirLightNum_; i++)
    {
        // ライトが有効なら設定を転送
        if (dirLights_[i].GetIsActive()) {
            cbLightGroup_.GetConstBuffMap()->dirLights_[i].isActive_ = 1;
            cbLightGroup_.GetConstBuffMap()->dirLights_[i].lightv_ = -dirLights_[i].GetLightDir();
            cbLightGroup_.GetConstBuffMap()->dirLights_[i].lightcolor_ = dirLights_[i].GetLightColor();
        }
        // ライトが無効なら転送しない。
        else {
            cbLightGroup_.GetConstBuffMap()->dirLights_[i].isActive_ = 0;
        }
    }
}

void LightGroup::DefaultLightSetting(void)
{
    dirLights_[0].SetIsActive(true);
    dirLights_[0].SetLightColor({ 1.f,0.f,0.f });
    dirLights_[0].SetLightDir({ 0.f,-1.f,0.f });

    dirLights_[1].SetIsActive(true);
    dirLights_[1].SetLightColor({ 0.f,1.f,0.f });
    dirLights_[1].SetLightDir({ +0.5f,+0.1f,+0.2f });

    dirLights_[2].SetIsActive(true);
    dirLights_[2].SetLightColor({ 0.f,0.f,1.f });
    dirLights_[2].SetLightDir({ -0.5f,+0.1f,-0.2f });
}

void LightGroup::DebugImGui(void)
{
    ImGui::Begin("dirLights");
    static float ambientColor0[3]{ 1,1,1 };
    static float lightDir0[3]{ 0,0,1 };
    static float lightColor0[3]{ 1,0,0 };
    ImGui::ColorEdit3("ambientColor", ambientColor0, ImGuiColorEditFlags_Float);
    ImGui::InputFloat3("lightDir", lightDir0);
    ImGui::ColorEdit3("lightColor", lightColor0, ImGuiColorEditFlags_Float);
    ImGui::End();
}

void LightGroup::SetAmbientColor(const Vector3& color)
{
    ambientColor_ = color;
    isDirty_ = true;
}

void LightGroup::SetLightDir(size_t index, const Vector3& lightDir)
{
    dirLights_[index].SetLightDir(lightDir);
    isDirty_ = true;
}

void LightGroup::SetDirLightColor(size_t index, const Vector3& lightColor)
{
    dirLights_[index].SetLightColor(lightColor);
    isDirty_ = true;
}

