#pragma once
#include <random>
#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"

static std::random_device sSeed_gen;
static std::mt19937 sEngine(sSeed_gen());

namespace Math {
    constexpr float kPI{ 3.14159265f };

    namespace Function {
        inline constexpr float ToRadian(float fDegrees) { return fDegrees * Math::kPI / 180.0f; }
        inline constexpr float ToDegree(float fRadians) { return fRadians * 180.0f / Math::kPI; }

        template<typename T>
        inline void LoopIncrement(T& value, T min, T max) {
            value++;
            if (value > max) value = min;
        }

        template<typename T>
        inline void LoopDecrement(T& value, T min, T max) {
            value--;
            if (value < min) value = max;
        }

        template<typename T> // wèµ½^Ì_ÈlðÔp
        inline T Random(double min, double max) {

            std::uniform_real_distribution<> dist1(min, max);

            return static_cast<T>(dist1(sEngine));
        }

        template<typename T>
        inline T Clamp(T value, T min, T max) {
            if (value < min) {
                return min;
            }
            else if (value > max) {
                return max;
            }
            else {
                return value;
            }
        }

        template<class T> // ³ = 1, 0 = 0,  = -1
        inline int32_t Sgn(T arg_v) {
            if (arg_v == 0) return 0;
            return arg_v > 0 ? 1 : -1;
        }

        template<class T> // ³ = 0 ~ 1,  = -1
        inline int32_t SgnA(T arg_v) {
            return arg_v >= 0 ? 1 : -1;
        }

        template<class T>
        inline T Cotangent(T arg_v) {
            return 1 / std::tan(arg_v);
        }

        // x:®ar, y:ÜxÆ, z:oxÓ
        Vector3 ToSphericalCoordinate(const Vector3& rectAngularCoordiante); 
        Vector3 ToRectAngularCoordinate(const Vector3& sphericalCoordinate);
    }

    namespace Ease {
        // In
        float EaseInSine(float t, float start = 0.f, float end = 1.f);
        float EaseInCubic(float t, float start = 0.f, float end = 1.f);
        float EaseInQuint(float t, float start = 0.f, float end = 1.f);
        float EaseInCirc(float t, float start = 0.f, float end = 1.f);
        float EaseInElastic(float t, float start = 0.f, float end = 1.f);

        // Out
        float EaseOutSine(float t, float start = 0.f, float end = 1.f);
        float EaseOutCubic(float t, float start = 0.f, float end = 1.f);
        float EaseOutQuint(float t, float start = 0.f, float end = 1.f);
        float EaseOutCirc(float t, float start = 0.f, float end = 1.f);
        float EaseOutElastic(float t, float start = 0.f, float end = 1.f);

        // InOut
        float EaseInOutSine(float t, float start = 0.f, float end = 1.f);
        float EaseInOutCubic(float t, float start = 0.f, float end = 1.f);
        float EaseInOutQuint(float t, float start = 0.f, float end = 1.f);
        float EaseInOutCirc(float t, float start = 0.f, float end = 1.f);
        float EaseInOutElastic(float t, float start = 0.f, float end = 1.f);
    }
}