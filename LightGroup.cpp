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
    // 環境光
    cbLightGroup_.GetConstBuffMap()->ambientColor = ambientColor_;

    // 平行光源
    for (size_t i = 0; i < kDirLightNum_; i++)
    {
        // ライトが有効なら設定を転送
        if (dirLights_[i].GetIsActive()) {
            cbLightGroup_.GetConstBuffMap()->dirLights[i].isActive = 1;
            cbLightGroup_.GetConstBuffMap()->dirLights[i].lightV = -dirLights_[i].GetLightDir();
            cbLightGroup_.GetConstBuffMap()->dirLights[i].lightColor = dirLights_[i].GetLightColor();
        }
        // ライトが無効なら転送しない。
        else {
            cbLightGroup_.GetConstBuffMap()->dirLights[i].isActive = 0;
        }
    }

    // 点光源
    for (size_t i = 0; i < kPointLightNum_; i++)
    {
        // ライトが有効なら設定を転送
        if (pointLights_[i].GetIsActive()) {
            cbLightGroup_.GetConstBuffMap()->pointLights[i].isActive = 1;
            cbLightGroup_.GetConstBuffMap()->pointLights[i].lightPos = pointLights_[i].GetLightPos();
            cbLightGroup_.GetConstBuffMap()->pointLights[i].lightColor = pointLights_[i].GetLightColor();
            cbLightGroup_.GetConstBuffMap()->pointLights[i].lightAtten = pointLights_[i].GetLightAtten();
        }
        // ライトが無効なら転送しない。
        else {
            cbLightGroup_.GetConstBuffMap()->pointLights[i].isActive = 0;
        }
    }

    // スポットライト
    for (size_t i = 0; i < kSpotLightNum_; i++)
    {
        // ライトが有効なら設定を転送
        if (spotLights_[i].GetIsActive()) {
            cbLightGroup_.GetConstBuffMap()->spotLights[i].isActive = 1;
            cbLightGroup_.GetConstBuffMap()->spotLights[i].lightV = -spotLights_[i].GetLightDir();
            cbLightGroup_.GetConstBuffMap()->spotLights[i].lightPos = spotLights_[i].GetLightPos();
            cbLightGroup_.GetConstBuffMap()->spotLights[i].lightColor = spotLights_[i].GetLightColor();
            cbLightGroup_.GetConstBuffMap()->spotLights[i].lightAtten = spotLights_[i].GetLightAtten();
            cbLightGroup_.GetConstBuffMap()->spotLights[i].lightFactorAngleCos = spotLights_[i].GetLightFactorAngleCos();
        }
        // ライトが無効なら転送しない。
        else {
            cbLightGroup_.GetConstBuffMap()->spotLights[i].isActive = 0;
        }
    }

    // 丸影
    for (size_t i = 0; i < kCircleShadowNum_; i++)
    {
        // 有効なら設定を転送
        if (circleShadows_[i].GetIsActive()) {
            cbLightGroup_.GetConstBuffMap()->circleShadows[i].isActive = 1;
            cbLightGroup_.GetConstBuffMap()->circleShadows[i].dir = -circleShadows_[i].GetDir();
            cbLightGroup_.GetConstBuffMap()->circleShadows[i].casterPos = circleShadows_[i].GetCasterPos();
            cbLightGroup_.GetConstBuffMap()->circleShadows[i].distanceCasterLight = circleShadows_[i].GetDistanceCasterLight();
            cbLightGroup_.GetConstBuffMap()->circleShadows[i].atten = circleShadows_[i].GetAtten();
            cbLightGroup_.GetConstBuffMap()->circleShadows[i].factorAngleCos = circleShadows_[i].GetFactorAngleCos();
        }
        // ライトが無効なら転送しない。
        else {
            cbLightGroup_.GetConstBuffMap()->spotLights[i].isActive = 0;
        }
    }
}

void LightGroup::DefaultLightSetting(void)
{
    dirLights_[0].SetIsActive(true);
    dirLights_[0].SetLightColor({ 1.f,1.f,1.f });
    dirLights_[0].SetLightDir({ 0.f,-1.f,0.f });

    //dirLights_[1].SetIsActive(true);
    //dirLights_[1].SetLightColor({ 0.f,1.f,0.f });
    //dirLights_[1].SetLightDir({ +0.5f,+0.1f,+0.2f });

    //dirLights_[2].SetIsActive(true);
    //dirLights_[2].SetLightColor({ 0.f,0.f,1.f });
    //dirLights_[2].SetLightDir({ -0.5f,+0.1f,-0.2f });

    //pointLights_[0].SetIsActive(true);
    //pointLights_[0].SetLightPos({ 2.0f, 2.f, -2.f });
    //pointLights_[0].SetLightColor({ 1.f,1.f,1.f });
    //pointLights_[0].SetLightAtten({ 0.3f,0.1f,0.1f });

    //spotLights_[0].SetIsActive(true);
    //spotLights_[0].SetLightDir({ 0.0f, -1.f, 0.f });
    //spotLights_[0].SetLightPos({ 0.0f, 5.f, 0.f });
    //spotLights_[0].SetLightColor({ 1.f,1.f,1.f });
    //spotLights_[0].SetLightAtten({ 0.f,0.f,0.f });
    //spotLights_[0].SetLightFactorAngle({ 20.f,30.f });

    circleShadows_[0].SetIsActive(true);
    circleShadows_[0].SetDir({ 0,-1,0 });
    circleShadows_[0].SetAtten({0.5f,0.6f,0.f});
    circleShadows_[0].SetFactorAngle({0.f,0.5f});
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

void LightGroup::SetPointLightPos(size_t index, const Vector3& lightPos)
{
    pointLights_[index].SetLightPos(lightPos);
    isDirty_ = true;
}

void LightGroup::SetPointLightColor(size_t index, const Vector3& lightColor)
{
    pointLights_[index].SetLightColor(lightColor);
    isDirty_ = true;
}

void LightGroup::SetPointLightAtten(size_t index, const Vector3& lightAtten)
{
    pointLights_[index].SetLightAtten(lightAtten);
    isDirty_ = true;
}

void LightGroup::SetSpotLightDir(size_t index, const Vector3& lightDir)
{
    spotLights_[index].SetLightDir(lightDir);
    isDirty_ = true;
}

void LightGroup::SetSpotLightPos(size_t index, const Vector3& lightPos)
{
    spotLights_[index].SetLightPos(lightPos);
    isDirty_ = true;
}

void LightGroup::SetSpotLightColor(size_t index, const Vector3& lightColor)
{
    spotLights_[index].SetLightColor(lightColor);
    isDirty_ = true;
}

void LightGroup::SetSpotLightAtten(size_t index, const Vector3& lightAtten)
{
    spotLights_[index].SetLightAtten(lightAtten);
    isDirty_ = true;
}

void LightGroup::SetSpotLightFactorAngle(size_t index, const Vector2& lightFactorAngle)
{
    spotLights_[index].SetLightFactorAngle(lightFactorAngle);
    isDirty_ = true;
}

void LightGroup::SetCircleShadowDir(size_t index, const Vector3& dir)
{
    circleShadows_[index].SetDir(dir);
    isDirty_ = true;
}

void LightGroup::SetCircleShadowAtten(size_t index, const Vector3& atten)
{
    circleShadows_[index].SetAtten(atten);
    isDirty_ = true;
}

void LightGroup::SetCircleShadowCasterPos(size_t index, const Vector3& casterPos)
{
    circleShadows_[index].SetCasterPos(casterPos);
    isDirty_ = true;
}

void LightGroup::SetCircleShadowDistanceCasterLight(size_t index, float distanceCasterLight)
{
    circleShadows_[index].SetDisctanceCasterLight(distanceCasterLight);
    isDirty_ = true;
}

