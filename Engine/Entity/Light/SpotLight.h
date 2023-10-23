#pragma once
#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
#include "MathUtil.h"

class SpotLight
{
private:
    //>> 定義
    typedef float (*mf)(float);
    const mf toRad = Math::Function::ToRadian;

public:
    struct CBSpotLight_t
    {
        Vector3 lightV;
        float pad1;
        Vector3 lightPos;
        float pad2;
        Vector3 lightColor;
        float pad3;
        Vector3 lightAtten;
        float pad4;
        Vector2 lightFactorAngleCos;
        uint32_t isActive;
        float pad5;
    };

private:
    //>> 変数
    Vector3 direction_{};
    Vector3 position_{};
    Vector3 color_{};
    Vector3 atten_{};
    Vector2 factorAngleCos_{};
    bool is_active_{};

public:
    //>> setter・getter
    void SetLightDir(const Vector3& arg_lightDir) { direction_ = arg_lightDir.Normalize(); }
    void SetLightPos(const Vector3& arg_lightPos) { position_ = arg_lightPos; }
    void SetLightColor(const Vector3& arg_lightColor) { color_ = arg_lightColor; }
    void SetLightAtten(const Vector3& arg_lightAtten) { atten_ = arg_lightAtten; }
    void SetLightFactorAngle(const Vector2& arg_lightFactorAngle) { factorAngleCos_ = { std::cosf(toRad(arg_lightFactorAngle.x)), std::cosf(toRad(arg_lightFactorAngle.y)) }; }
    void SetIsActive(bool arg_isActive) { is_active_ = arg_isActive; }
   
    const Vector3& GetLightDir(void) { return direction_; }
    const Vector3& GetLightPos(void) { return position_; }
    const Vector3& GetLightColor(void) { return color_; }
    const Vector3& GetLightAtten(void) { return atten_; }
    const Vector2& GetLightFactorAngleCos(void) { return factorAngleCos_; }
    bool GetIsActive(void) { return is_active_; }
};
