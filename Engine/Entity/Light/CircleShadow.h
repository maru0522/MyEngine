#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "MathUtil.h"

class CircleShadow
{
private:
    // 定義
    typedef float (*mf)(float);
    const mf toRad = Math::Function::ToRadian;

public:
    struct CBCircleShadow_t
    {
        Vector3 dir;
        float pad1;
        Vector3 casterPos;
        float distanceCasterLight;
        Vector3 atten;
        float pad2;
        Vector2 factorAngleCos;
        uint32_t isActive;
        float pad3;
    };

private:
    // 変数
    Vector3 dir_{ 1,0,0 };
    Vector3 casterPos_{ 0,0,0 };
    Vector3 atten_{ 0.5f,0.6f,0.f };
    Vector2 factorAngleCos_{ 0.2f,0.5f };
    float distanceCasterLight_{ 100.f };
    bool isActive_{};

public:
    // setter・getter
    inline void SetAtten(const Vector3& atten) { atten_ = atten; }
    inline void SetIsActive(bool isActive) { isActive_ = isActive; }
    inline void SetDir(const Vector3& dir) { dir_ = dir.Normalize(); }
    inline void SetCasterPos(const Vector3& casterPos) { casterPos_ = casterPos; }
    inline void SetDisctanceCasterLight(float distanceCasterLight) { distanceCasterLight_ = distanceCasterLight; }
    inline void SetFactorAngle(const Vector2& factorAngle) { factorAngleCos_ = { std::cosf(toRad(factorAngle.x)), std::cosf(toRad(factorAngle.y)) }; }

    inline bool GetIsActive(void) { return isActive_; }
    inline const Vector3& GetDir(void) { return dir_; }
    inline const Vector3& GetAtten(void) { return atten_; }
    inline const Vector3& GetCasterPos(void) { return casterPos_; }
    inline const Vector2& GetFactorAngleCos(void) { return factorAngleCos_; }
    inline float GetDistanceCasterLight(void) { return distanceCasterLight_; }
};
