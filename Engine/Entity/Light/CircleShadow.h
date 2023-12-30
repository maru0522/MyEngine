#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "MathUtil.h"
/**
 * @file CircleShadow.h
 * @brief 丸影についてのクラスが宣言されたファイル
 */

class CircleShadow
{
private:
    //>> 定義
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
    //>> 変数
    Vector3 direction_{};
    Vector3 position_caster_{};
    Vector3 atten_{};
    Vector2 factorAngleCos_{};
    float distance_caster_{};
    bool is_active_{};

public:
    // setter・getter
    inline void SetAtten(const Vector3& arg_atten) { atten_ = arg_atten; }
    inline void SetIsActive(bool arg_isActive) { is_active_ = arg_isActive; }
    inline void SetDir(const Vector3& arg_dir) { direction_ = arg_dir.Normalize(); }
    inline void SetCasterPos(const Vector3& arg_casterPos) { position_caster_ = arg_casterPos; }
    inline void SetDisctanceCasterLight(float arg_distanceCasterLight) { distance_caster_ = arg_distanceCasterLight; }
    inline void SetFactorAngle(const Vector2& arg_factorAngle) { factorAngleCos_ = { std::cosf(toRad(arg_factorAngle.x)), std::cosf(toRad(arg_factorAngle.y)) }; }

    inline bool GetIsActive(void) { return is_active_; }
    inline const Vector3& GetDir(void) { return direction_; }
    inline const Vector3& GetAtten(void) { return atten_; }
    inline const Vector3& GetCasterPos(void) { return position_caster_; }
    inline const Vector2& GetFactorAngleCos(void) { return factorAngleCos_; }
    inline float GetDistanceCasterLight(void) { return distance_caster_; }
};
