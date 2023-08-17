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

static std::random_device sSeed_gen;
static std::mt19937 sEngine(sSeed_gen());

namespace Math {
    constexpr float kPI = std::numbers::pi_v<float>;

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

        template<typename T>
        void Loop(T& value, T min, T max) {
            if (value < min) value = max;
            else if (value > max) value = min;
        }

        template<typename T> // �w�肵���^�̃����_���Ȓl��ԋp
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

        // �l���w�肵���l���ɓ����Ă邩
        template<typename T>
        inline bool InRange(T value, T min, T max) {
            return min < value && value < max;
        }

        // ����: ���l,���z�l,�ɏ���
        // ���l�����z�l����}�ɏ����͈͓̔��ł��鎞�A���z�l���Ԃ����B
        template<typename T>
        inline T FixEpsilon(T fixValue, T idealValue, T epsilon) {
            if (idealValue - epsilon < fixValue && fixValue < idealValue + epsilon) fixValue = idealValue;
            return fixValue;
        }

        template<class T> // �� = 1, 0 = 0, �� = -1
        inline int32_t Sgn(T arg_v) {
            if (arg_v == 0) return 0;
            return arg_v > 0 ? 1 : -1;
        }

        template<class T> // �� = 0 ~ 1, �� = -1
        inline int32_t SgnA(T arg_v) {
            return arg_v >= 0 ? 1 : -1;
        }

        template<class T>
        inline T Cotangent(T arg_v) {
            return 1 / std::tan(arg_v);
        }

        // ���ʍ��W�n���f�J���g���W�n�ɕϊ�����   r:���ar, theta:�ܓx��, phi:�o�x��
        Vector3 ToCartesian(float r, float theta, float phi);
        //Vector3 ToSphericalCoordinate(const Vector3& rectAngularCoordiante);

        // �A�t�B���ϊ�
        Matrix4 AffinTrans(const Transform& transform);
        Matrix4 AffinTrans(const Transform& transform, const Axis3& axes);
        Matrix4 AffinTrans(const Vector3& pos, const Vector3& scale, const Vector3& rotEular);
        Matrix4 AffinTrans(const Vector3& pos, const Vector3& scale, const Axis3& axes);
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