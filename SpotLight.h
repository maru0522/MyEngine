#pragma once
#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
#include "MathUtil.h"

class SpotLight
{
private:
    // íËã`
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
    // ïœêî
    Vector3 lightDir_{ 1,0,0 };
    Vector3 lightPos_{ 0,0,0 };
    Vector3 lightColor_{ 1,1,1 };
    Vector3 lightAtten_{ 1.f,1.f,1.f };
    Vector2 lightFactorAngleCos_{ 0.5f,0.2f };

    bool isActive_{};

public:
    // setterÅEgetter
    inline void SetLightDir(const Vector3& lightDir) { lightDir_ = lightDir.normalize(); }
    inline void SetLightPos(const Vector3& lightPos) { lightPos_ = lightPos; }
    inline void SetLightColor(const Vector3& lightcolor) { lightColor_ = lightcolor; }
    inline void SetLightAtten(const Vector3& lightAtten) { lightAtten_ = lightAtten; }
    inline void SetLightFactorAngle(const Vector2& lightFactorAngle) { lightFactorAngleCos_ = { std::cosf(toRad(lightFactorAngle.x)), std::cosf(toRad(lightFactorAngle.y)) }; }
    inline void SetIsActive(bool isActive) { isActive_ = isActive; }
    
    inline const Vector3& GetLightDir(void) { return lightDir_; }
    inline const Vector3& GetLightPos(void) { return lightPos_; }
    inline const Vector3& GetLightColor(void) { return lightColor_; }
    inline const Vector3& GetLightAtten(void) { return lightAtten_; }
    inline const Vector2& GetLightFactorAngleCos(void) { return lightFactorAngleCos_; }
    inline bool GetIsActive(void) { return isActive_; }
};