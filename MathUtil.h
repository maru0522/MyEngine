#pragma once
#include <random>
#include <cmath>

static std::random_device sSeed_gen;
static std::mt19937 sEngine(sSeed_gen());

namespace Math {
    constexpr float PI{ 3.14159265f };

    namespace Function {
        inline constexpr float ToRadian(float fDegrees) { return fDegrees * Math::PI / 180.0f; }
        inline constexpr float ToDegree(float fRadians) { return fRadians * 180.0f / Math::PI; }

        template<typename T> // 指定した型のランダムな値を返却
        inline const T Random(const double min, const double max) {

            std::uniform_real_distribution<> dist1(min, max);

            return static_cast<T>(dist1(sEngine));
        }

        template<typename T> // プリミティブ型なら参照は無い方が早い？？
        inline const T& Clamp(const T& value, const T& min, const T& max) {
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

        template<class T> // 正 = 1, 0 = 0, 負 = -1
        inline int32_t Sgn(const T arg_v) {
            if (arg_v == 0) return 0;
            return arg_v > 0 ? 1 : -1;
        }

        template<class T>
        inline T Cotangent(T arg_v) {
            return 1 / std::tan(arg_v);
        }
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