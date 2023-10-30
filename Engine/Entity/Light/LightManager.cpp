#include "LightManager.h"
#include <imgui.h>
#include "Util.h"

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

int32_t LightManager::UsableRightNum(LightType arg_type)
{
    // 平行光源
    if (arg_type == LightType::DIRECTIONAL)
    {
        for (int32_t i = 0; i < kDirLightNum_; i++)
        {
            if (lights_directional_[i].GetIsActive() == false) { return i; }
        }
    }

    // 点光源
    if (arg_type == LightType::POINT)
    {
        for (int32_t i = 0; i < kPointLightNum_; i++)
        {
            if (lights_point_[i].GetIsActive() == false) { return i; }
        }
    }

    // スポットライト
    if (arg_type == LightType::SPOT)
    {
        for (int32_t i = 0; i < kSpotLightNum_; i++)
        {
            if (lights_spot[i].GetIsActive() == false) { return i; }
        }
    }

    // 丸影
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        for (int32_t i = 0; i < kCircleShadowNum_; i++)
        {
            if (lights_circleShadow_[i].GetIsActive() == false) { return i; }
        }
    }

    return -1;
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
            cb_lightGroup_.GetConstBuffMap()->dirLights[i].isActive = false;
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
            cb_lightGroup_.GetConstBuffMap()->pointLights[i].isActive = false;
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
            cb_lightGroup_.GetConstBuffMap()->spotLights[i].isActive = false;
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
            cb_lightGroup_.GetConstBuffMap()->circleShadows[i].isActive = false;
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

void LightManager::SetLightActive(LightType arg_type, size_t arg_index, bool arg_isActive)
{
    // 指定されたタイプが [平行光源]
    if (arg_type == LightType::DIRECTIONAL)
    {
        // index番目の、平行光源の動作状況を設定
        lights_directional_[arg_index].SetIsActive(arg_isActive);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [点光源]
    if (arg_type == LightType::POINT)
    {
        // index番目の、点光源の動作状況を設定
        lights_point_[arg_index].SetIsActive(arg_isActive);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [スポットライト]
    if (arg_type == LightType::SPOT)
    {
        // index番目の、スポットライトの動作状況を設定
        lights_spot[arg_index].SetIsActive(arg_isActive);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [丸影]
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        // index番目の、丸影の動作状況を設定
        lights_circleShadow_[arg_index].SetIsActive(arg_isActive);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }
}

void LightManager::SetLightPos(LightType arg_type, size_t arg_index, const Vector3& arg_pos)
{
    // 指定されたタイプが [平行光源]
    if (arg_type == LightType::DIRECTIONAL)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Directional_Lightは、メンバ変数にpositionを含んでいないため座標を指定できませんでした。");
        return;
    }

    // 指定されたタイプが [点光源]
    if (arg_type == LightType::POINT)
    {
        // index番目の、点光源の座標を設定
        lights_point_[arg_index].SetLightPos(arg_pos);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [スポットライト]
    if (arg_type == LightType::SPOT)
    {
        // index番目の、スポットライトの座標を設定
        lights_spot[arg_index].SetLightPos(arg_pos);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [丸影]
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        // index番目の、丸影のキャスター座標を設定
        lights_circleShadow_[arg_index].SetCasterPos(arg_pos);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }
}

void LightManager::SetLightDir(LightType arg_type, size_t arg_index, const Vector3& arg_dir)
{
    // 指定されたタイプが [平行光源]
    if (arg_type == LightType::DIRECTIONAL)
    {
        // index番目の、平行光源の向きを設定
        lights_directional_[arg_index].SetLightDir(arg_dir);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [点光源]
    if (arg_type == LightType::POINT)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Point_Lightは、メンバ変数にdirectionを含んでいないため向きを指定できませんでした。");
        return;
    }

    // 指定されたタイプが [スポットライト]
    if (arg_type == LightType::SPOT)
    {
        // index番目の、スポットライトの向きを設定
        lights_spot[arg_index].SetLightDir(arg_dir);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [丸影]
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        // index番目の、丸影の向きを設定
        lights_circleShadow_[arg_index].SetDir(arg_dir);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }
}

void LightManager::SetLightColor(LightType arg_type, size_t arg_index, const Vector3& arg_color)
{
    // 指定されたタイプが [平行光源]
    if (arg_type == LightType::DIRECTIONAL)
    {
        // index番目の、平行光源の色を設定
        lights_directional_[arg_index].SetLightColor(arg_color);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [点光源]
    if (arg_type == LightType::POINT)
    {
        // index番目の、点光源の色を設定
        lights_point_[arg_index].SetLightColor(arg_color);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [スポットライト]
    if (arg_type == LightType::SPOT)
    {
        // index番目の、スポットライトの色を設定
        lights_spot[arg_index].SetLightColor(arg_color);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [丸影]
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Circle_Shadowは、メンバ変数にcolorを含んでいないため色を指定できませんでした。");
        return;
    }
}

void LightManager::SetLightAtten(LightType arg_type, size_t arg_index, const Vector3& arg_atten)
{
    // 指定されたタイプが [平行光源]
    if (arg_type == LightType::DIRECTIONAL)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Directional_Lightは、メンバ変数にattenuationを含んでいないため減衰値を指定できませんでした。");
        return;
    }

    // 指定されたタイプが [点光源]
    if (arg_type == LightType::POINT)
    {
        // index番目の、点光源の減衰値を設定
        lights_point_[arg_index].SetLightAtten(arg_atten);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [スポットライト]
    if (arg_type == LightType::SPOT)
    {
        // index番目の、スポットライトの減衰値を設定
        lights_spot[arg_index].SetLightAtten(arg_atten);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [丸影]
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        // index番目の、丸影の減衰値を設定
        lights_circleShadow_[arg_index].SetAtten(arg_atten);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }
}

void LightManager::SetLightFactorAngle(LightType arg_type, size_t arg_index, const Vector2& arg_atten)
{
    // 指定されたタイプが [平行光源]
    if (arg_type == LightType::DIRECTIONAL)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Directional_Lightは、メンバ変数にfactorAngleを含んでいないため減衰角度を指定できませんでした。");
        return;
    }

    // 指定されたタイプが [点光源]
    if (arg_type == LightType::POINT)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Point_Lightは、メンバ変数にfactorAngleを含んでいないため減衰角度を指定できませんでした。");
        return;
    }

    // 指定されたタイプが [スポットライト]
    if (arg_type == LightType::SPOT)
    {
        // index番目の、スポットライトの減衰角度を設定
        lights_spot[arg_index].SetLightFactorAngle(arg_atten);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }

    // 指定されたタイプが [丸影]
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        // index番目の、丸影の減衰角度を設定
        lights_circleShadow_[arg_index].SetFactorAngle(arg_atten);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }
}

void LightManager::SetLightDistanceAtCaster(LightType arg_type, size_t arg_index, float arg_distanceAtCaster)
{
    // 指定されたタイプが [平行光源]
    if (arg_type == LightType::DIRECTIONAL)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Directional_Lightは、メンバ変数にdistanceCasterを含んでいないためキャスターまでの距離を指定できませんでした。");
        return;
    }

    // 指定されたタイプが [点光源]
    if (arg_type == LightType::POINT)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Point_Lightは、メンバ変数にdistanceCasterを含んでいないためキャスターまでの距離を指定できませんでした。");
        return;
    }

    // 指定されたタイプが [スポットライト]
    if (arg_type == LightType::SPOT)
    {
        // 設定出来ないのでログを出力
        Util::Log::PrintOutputWindow("[LightManager] : Spot_Lightは、メンバ変数にdistanceCasterを含んでいないためキャスターまでの距離を指定できませんでした。");
        return;
    }

    // 指定されたタイプが [丸影]
    if (arg_type == LightType::CIRCLE_SHADOW)
    {
        // index番目の、丸影のキャスターまでの距離を設定
        lights_circleShadow_[arg_index].SetDisctanceCasterLight(arg_distanceAtCaster);
        // 汚染フラグを立てる
        is_dirty_ = true;
        return;
    }
}
