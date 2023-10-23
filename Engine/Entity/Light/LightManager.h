#pragma once
#include <cstdint>
#include "Vector3.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "ConstBuffer.h"
#include "CircleShadow.h"
#include "ImguiController.h"
#include "DirectionalLight.h"

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

private:
    void TransferCB(void);
    void DefaultLightSetting(void);
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
    //>> setter・getter
    void SetAmbientColor(const Vector3& color);

    void SetLightDir(size_t index, const Vector3& lightDir);
    void SetDirLightColor(size_t index, const Vector3& lightColor);
    inline void SetDirLightActive(size_t index, bool isActive) { lights_directional_[index].SetIsActive(isActive); }

    void SetPointLightPos(size_t index, const Vector3& lightPos);
    void SetPointLightColor(size_t index, const Vector3& lightColor);
    void SetPointLightAtten(size_t index, const Vector3& lightAtten);
    inline void SetPointLightActive(size_t index, bool isActive) { lights_point_[index].SetIsActive(isActive); }

    void SetSpotLightDir(size_t index, const Vector3& lightDir);
    void SetSpotLightPos(size_t index, const Vector3& lightPos);
    void SetSpotLightColor(size_t index, const Vector3& lightColor);
    void SetSpotLightAtten(size_t index, const Vector3& lightAtten);
    void SetSpotLightFactorAngle(size_t index, const Vector2& lightFactorAngle);
    inline void SetSpotLightActive(size_t index, bool isActive) { lights_spot[index].SetIsActive(isActive); }

    void SetCircleShadowDir(size_t index, const Vector3& dir);
    void SetCircleShadowAtten(size_t index, const Vector3& atten);
    void SetCircleShadowCasterPos(size_t index, const Vector3& casterPos);
    void SetCircleShadowDistanceCasterLight(size_t index, float distanceCasterLight);
    void SetCircleShadowFactorAngle(size_t index, const Vector2& factorAngle);
    inline void SetCircleShadowActive(size_t index, bool isActive) { lights_circleShadow_[index].SetIsActive(isActive); }

    bool GetCircleShadowActive(size_t index) { return lights_circleShadow_[index].GetIsActive(); }
};

