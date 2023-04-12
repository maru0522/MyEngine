#pragma once
#include <random>

static std::random_device seed_gen;
static std::mt19937 engine(seed_gen());

namespace Math {
    constexpr float PI{ 3.14159265f };

    namespace Function {
        inline constexpr float ToRadian(float fDegrees) { return fDegrees * Math::PI / 180.0f; }
        inline constexpr float ToDegree(float fRadians) { return fRadians * 180.0f / Math::PI; }

        template<typename T> // 指定した型のランダムな値を返却
        inline const T Random(const double min, const double max) {

            std::uniform_real_distribution<> dist1(min, max);

            return static_cast<T>(dist1(engine));
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
        inline int Sign(const T arg_v) {
            if (arg_v == 0) return 0;
            return arg_v > 0 ? 1 : -1;
        }
    }
}