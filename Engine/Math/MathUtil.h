#pragma once
#include <cmath>
#include <random>
#include <numbers>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Transform.h"
#include "Quaternion.h"
/**
 * @file MathUtil.h
 * @brief 自作数学関数が羅列して宣言されたファイル
 */

static std::random_device sSeed_gen;
static std::mt19937 sEngine(sSeed_gen());

namespace Math {
    constexpr float kPI = std::numbers::pi_v<float>;

    namespace Function {
        inline constexpr float ToRadian(float fDegrees) { return fDegrees * Math::kPI / 180.0f; }
        inline constexpr float ToDegree(float fRadians) { return fRadians * 180.0f / Math::kPI; }

        // xとy、それぞれでより値の小さい方を選択した、Vector2を返す
        const Vector2 Min(const Vector2& v1, const Vector2& v2);

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

        template<typename T>
        void Loop(T& value, T min, T max) {
            if (value < min) value = max;
            else if (value > max) value = min;
        }

        template<typename T> // 指定した型のランダムな値を返却
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

        // 値が指定した値内に入ってるか
        template<typename T>
        inline bool InRange(T value, T min, T max) {
            return min < value && value < max;
        }

        // 引数: 実値,理想値,極小数
        // 実値が理想値から±極小数の範囲内である時、理想値が返される。
        template<typename T>
        inline T FixEpsilon(T fixValue, T idealValue, T epsilon) {
            if (idealValue - epsilon < fixValue && fixValue < idealValue + epsilon) fixValue = idealValue;
            return fixValue;
        }

        template<class T> // 正 = 1, 0 = 0, 負 = -1
        inline int32_t Sgn(T arg_v) {
            if (arg_v == 0) return 0;
            return arg_v > 0 ? 1 : -1;
        }

        template<class T> // 正 = 0 ~ 1, 負 = -1
        inline int32_t SgnA(T arg_v) {
            return arg_v >= 0 ? 1 : -1;
        }

        template<class T>
        inline T Cotangent(T arg_v) {
            return 1 / std::tan(arg_v);
        }

        // 球面座標系をデカルト座標系に変換する   r:動径r, theta:緯度θ, phi:経度φ
        Vector3 ToCartesian(float r, float theta, float phi);
        //Vector3 ToSphericalCoordinate(const Vector3& rectAngularCoordiante);

        Vector2 WorldToScreen(const Matrix4& arg_matWorld, int32_t arg_scW, int32_t arg_scH, const Matrix4& arg_view, const Matrix4& arg_prj);
        Vector3 ScreenToWorld(const Vector2& arg_scPos, float arg_fZ, int32_t arg_scW, int32_t arg_scH, const Matrix4& arg_view, const Matrix4& arg_prj);

        // アフィン変換
        Matrix4 AffinTrans(const Transform& arg_transform, TransformMatrix* arg_transMatPtr = nullptr);
        Matrix4 AffinTrans(const Transform& arg_transform, const Axis3& arg_axes, TransformMatrix* arg_transMatPtr = nullptr);
        Matrix4 AffinTrans(const Transform& arg_transform, const Axis3& arg_axes, const Vector3& arg_addRot, TransformMatrix* arg_transMatPtr = nullptr);
        Matrix4 AffinTrans(const Vector3& arg_pos, const Vector3& arg_scale, const Vector3& arg_rotEular, TransformMatrix* arg_transMatPtr = nullptr);
        Matrix4 AffinTrans(const Vector3& arg_pos, const Vector3& arg_scale, const Axis3& arg_axes, TransformMatrix* arg_transMatPtr = nullptr);

        // スプライン曲線
        Vector3 Spline(const std::vector<Vector3>& arg_points, size_t startIndex, float timeRate);
    }

    namespace Ease { // float用
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

    namespace Ease3 { // Vector3用
        // In
        Vector3 EaseInSin(float t, const Vector3& start = { 0,0,0 }, const Vector3& end = { 1,1,1 });
        Vector3 EaseInCubic(float t, const Vector3& start = { 0,0,0 }, const Vector3& end = { 1,1,1 });

        // InOut
        Vector3 EaseInOutCubic(float t, const Vector3& start = { 0,0,0 }, const Vector3& end = { 1,1,1 });
    }
}
