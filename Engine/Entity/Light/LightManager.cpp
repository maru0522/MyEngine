#include "LightManager.h"
#include <imgui.h>

LightManager::LightManager(void)
{
    cb_lightGroup_.Create();
    TransferCB();
}

void LightManager::Update(void)
{
    // 値の更新があった時だけ定数バッファに転送する
    if (is_dirty_) {
        TransferCB();
        is_dirty_ = false;
    }
    //DebugImGui();
}

void LightManager::Draw(void)
{
    InitDirectX::GetInstance()->GetCommandList()->
        SetGraphicsRootConstantBufferView(4, cb_lightGroup_.GetBuffer()->GetGPUVirtualAddress());
}

void LightManager::TransferCB(void)
{
    // 環境光
    cb_lightGroup_.GetConstBuffMap()->ambientColor = ambientColor_;

    // 平行光源
    for (size_t i = 0; i < kDirLightNum_; i++)
    {
        // ライトが有効なら設定を転送
        if (lights_directional_[i].GetIsActive()) {
            cb_lightGroup_.GetConstBuffMap()->dirLights[i].isActive = 1;
            cb_lightGroup_.GetConstBuffMap()->dirLights[i].lightV = -lights_directional_[i].GetLightDir();
            cb_lightGroup_.GetConstBuffMap()->dirLights[i].lightColor = lights_directional_[i].GetLightColor();
        }
        // ライトが無効なら転送しない。
        else {
            cb_lightGroup_.GetConstBuffMap()->dirLights[i].isActive = 0;
        }
    }

    // 点光源
    for (size_t i = 0; i < kPointLightNum_; i++)
    {
        // ライトが有効なら設定を転送
        if (lights_point_[i].GetIsActive()) {
            cb_lightGroup_.GetConstBuffMap()->pointLights[i].isActive = 1;
            cb_lightGroup_.GetConstBuffMap()->pointLights[i].lightPos = lights_point_[i].GetLightPos();
            cb_lightGroup_.GetConstBuffMap()->pointLights[i].lightColor = lights_point_[i].GetLightColor();
            cb_lightGroup_.GetConstBuffMap()->pointLights[i].lightAtten = lights_point_[i].GetLightAtten();
        }
        // ライトが無効なら転送しない。
        else {
            cb_lightGroup_.GetConstBuffMap()->pointLights[i].isActive = 0;
        }
    }

    // スポットライト
    for (size_t i = 0; i < kSpotLightNum_; i++)
    {
        // ライトが有効なら設定を転送
        if (lights_spot[i].GetIsActive()) {
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].isActive = 1;
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].lightV = -lights_spot[i].GetLightDir();
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].lightPos = lights_spot[i].GetLightPos();
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].lightColor = lights_spot[i].GetLightColor();
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].lightAtten = lights_spot[i].GetLightAtten();
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].lightFactorAngleCos = lights_spot[i].GetLightFactorAngleCos();
        }
        // ライトが無効なら転送しない。
        else {
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].isActive = 0;
        }
    }

    // 丸影
    for (size_t i = 0; i < kCircleShadowNum_; i++)
    {
        // 有効なら設定を転送
        if (lights_circleShadow_[i].GetIsActive()) {
            cb_lightGroup_.GetConstBuffMap()->circleShadows[i].isActive = 1;
            cb_lightGroup_.GetConstBuffMap()->circleShadows[i].dir = -lights_circleShadow_[i].GetDir();
            cb_lightGroup_.GetConstBuffMap()->circleShadows[i].casterPos = lights_circleShadow_[i].GetCasterPos();
            cb_lightGroup_.GetConstBuffMap()->circleShadows[i].distanceCasterLight = lights_circleShadow_[i].GetDistanceCasterLight();
            cb_lightGroup_.GetConstBuffMap()->circleShadows[i].atten = lights_circleShadow_[i].GetAtten();
            cb_lightGroup_.GetConstBuffMap()->circleShadows[i].factorAngleCos = lights_circleShadow_[i].GetFactorAngleCos();
        }
        // ライトが無効なら転送しない。
        else {
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].isActive = 0;
        }
    }
}

void LightManager::DebugImGui(void)
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

void LightManager::SetAmbientColor(const Vector3& color)
{
    ambientColor_ = color;
    is_dirty_ = true;
}

void LightManager::SetLightDir(size_t index, const Vector3& lightDir)
{
    lights_directional_[index].SetLightDir(lightDir);
    is_dirty_ = true;
}

void LightManager::SetDirLightColor(size_t index, const Vector3& lightColor)
{
    lights_directional_[index].SetLightColor(lightColor);
    is_dirty_ = true;
}

void LightManager::SetPointLightPos(size_t index, const Vector3& lightPos)
{
    lights_point_[index].SetLightPos(lightPos);
    is_dirty_ = true;
}

void LightManager::SetPointLightColor(size_t index, const Vector3& lightColor)
{
    lights_point_[index].SetLightColor(lightColor);
    is_dirty_ = true;
}

void LightManager::SetPointLightAtten(size_t index, const Vector3& lightAtten)
{
    lights_point_[index].SetLightAtten(lightAtten);
    is_dirty_ = true;
}

void LightManager::SetSpotLightDir(size_t index, const Vector3& lightDir)
{
    lights_spot[index].SetLightDir(lightDir);
    is_dirty_ = true;
}

void LightManager::SetSpotLightPos(size_t index, const Vector3& lightPos)
{
    lights_spot[index].SetLightPos(lightPos);
    is_dirty_ = true;
}

void LightManager::SetSpotLightColor(size_t index, const Vector3& lightColor)
{
    lights_spot[index].SetLightColor(lightColor);
    is_dirty_ = true;
}

void LightManager::SetSpotLightAtten(size_t index, const Vector3& lightAtten)
{
    lights_spot[index].SetLightAtten(lightAtten);
    is_dirty_ = true;
}

void LightManager::SetSpotLightFactorAngle(size_t index, const Vector2& lightFactorAngle)
{
    lights_spot[index].SetLightFactorAngle(lightFactorAngle);
    is_dirty_ = true;
}

void LightManager::SetCircleShadowDir(size_t index, const Vector3& dir)
{
    lights_circleShadow_[index].SetDir(dir);
    is_dirty_ = true;
}

void LightManager::SetCircleShadowAtten(size_t index, const Vector3& atten)
{
    lights_circleShadow_[index].SetAtten(atten);
    is_dirty_ = true;
}

void LightManager::SetCircleShadowCasterPos(size_t index, const Vector3& casterPos)
{
    lights_circleShadow_[index].SetCasterPos(casterPos);
    is_dirty_ = true;
}

void LightManager::SetCircleShadowDistanceCasterLight(size_t index, float distanceCasterLight)
{
    lights_circleShadow_[index].SetDisctanceCasterLight(distanceCasterLight);
    is_dirty_ = true;
}

void LightManager::SetCircleShadowFactorAngle(size_t index, const Vector2& factorAngle)
{
    lights_circleShadow_[index].SetFactorAngle(factorAngle);
    is_dirty_ = true;
}

