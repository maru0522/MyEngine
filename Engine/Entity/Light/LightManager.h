#pragma once
#include <cstdint>
#include "Vector3.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "ConstBuffer.h"
#include "CircleShadow.h"
#include "ImguiController.h"
#include "DirectionalLight.h"

enum class LightType
{
    DIRECTIONAL,
    POINT,
    SPOT,
    CIRCLE_SHADOW,
};

class LightManager final
{
private:
    //>> 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    //>> 定数
    static constexpr uint32_t kDirLightNum_ = 3;
    static constexpr uint32_t kPointLightNum_ = 50;
    static constexpr uint32_t kSpotLightNum_ = 50;
    static constexpr uint32_t kCircleShadowNum_ = 50;

    struct CBLightGroup_t
    {
        Vector3 ambientColor; // 環境光の色
        float pad;
        DirectionalLight::CBDirLight_t dirLights[kDirLightNum_];         // 平行光源用
        PointLight::CBPointLight_t pointLights[kPointLightNum_];         // 点光源用
        SpotLight::CBSpotLight_t spotLights[kSpotLightNum_];             // スポットライト用
        CircleShadow::CBCircleShadow_t circleShadows[kCircleShadowNum_]; // 丸影用
    };

    //>> 関数
    LightManager(void);
    void Update(void);
    void Draw(void);

    int32_t UsableRightNum(LightType arg_type);

private:
    void TransferCB(void);
    void DebugImGui(void);

    //>> 変数
    ConstBuffer<CBLightGroup_t> cb_lightGroup_;
    Vector3 ambientColor_{ 1,1,1 };

    DirectionalLight lights_directional_[kDirLightNum_];
    PointLight lights_point_[kPointLightNum_];
    SpotLight lights_spot[kSpotLightNum_];
    CircleShadow lights_circleShadow_[kCircleShadowNum_];
    bool is_dirty_{};

public:
    //>> setter
    void SetAmbientColor(const Vector3& color);

    void SetLightActive(LightType arg_type, size_t arg_index, bool arg_isActive);
    void SetLightPos(LightType arg_type, size_t arg_index, const Vector3& arg_pos);
    void SetLightDir(LightType arg_type, size_t arg_index, const Vector3& arg_dir);
    void SetLightColor(LightType arg_type, size_t arg_index, const Vector3& arg_color);
    void SetLightAtten(LightType arg_type, size_t arg_index, const Vector3& arg_atten);
    void SetLightFactorAngle(LightType arg_type, size_t arg_index, const Vector2& arg_atten);
    void SetLightDistanceAtCaster(LightType arg_type, size_t arg_index, float arg_distanceAtCaster);

    //>> getter
    bool GetCircleShadowActive(size_t index) { return lights_circleShadow_[index].GetIsActive(); }
};

