#pragma once
#include <string>
#include <chrono>
#include <random>
#if _MSC_VER > 1922 && !defined(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>

static std::random_device seed_gen;
static std::mt19937 engine(seed_gen());

namespace Util {

    namespace Math {

        constexpr float PI{ 3.14159265f };

        template<typename T>
        inline const T Random(const double min, const double max) {

            std::uniform_real_distribution<> dist1(min, max);

            return static_cast<T>(dist1(engine));
        }

        template<typename T>
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
    }

    namespace Convert {
        inline constexpr float ToRadian(float fDegrees) { return fDegrees * Math::PI / 180.0f; }
        inline constexpr float ToDegree(float fRadians) { return fRadians * 180.0f / Math::PI; }

        std::string ToString(const std::wstring& wStr);
        std::string ToString(const char* cStr);
        std::wstring ToWString(const std::string& str);
        std::wstring ToWString(const char* cStr);
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

    class Timer {
    public:
        // ��`
        using nanoseconds = std::chrono::nanoseconds;
        using microseconds = std::chrono::microseconds;
        using milliseconds = std::chrono::milliseconds;
        using seconds = std::chrono::seconds;
        using minutes = std::chrono::minutes;
        using hours = std::chrono::hours;

        // �ÓI�֐�
        /// <summary>
        /// <para>Windows���N�����Ă���̌o�ߎ��Ԃ��~���b�P�ʂł���킵���l��Ԃ��B</para>
        /// <para>���������擾����킯�ł͂Ȃ����A���̊֐��̑��݈Ӌ`�͎��Ԍv���ɂ���B</para>
        /// <para>TimeUnit��std::chrono::duration�̕ʖ��ɂ���[���ԊԊu��\���^]</para>
        /// <para>�݂̂��g�p�ł���B</para>
        /// </summary>
        /// <typeparam name="TimeUnit"></typeparam>
        /// <returns></returns>
        template<class TimeUnit>
        static int32_t GetNowCount(void);
        static std::string GetNowDateTime(void);

        // �֐�
        void Start(float endTime = 1.0f);

        inline void SetEndTime(float endTime) { endTime_ = endTime; }

        inline const float GetEndTime(void) { return endTime_; }
        inline const bool GetIsEnd(void) { return endTime_ < (GetNowCount<milliseconds>() - startTime_) / 1000.0f; } // endTime < elapsedTime
        inline const float GetElapsedTime(void) { return float{ (GetNowCount<milliseconds>() - startTime_) / 1000.0f }; }

    private:
        // �ϐ�
        int32_t startTime_{ 0 }; // �v���J�n����
        float endTime_{ 0.f }; // ���B�ڕW����
    };

    namespace Log {
        void PrintOutputWindow(const std::string& str);
        void PrintExternalText(const std::string& str, const std::experimental::filesystem::path& logTextPath = std::experimental::filesystem::current_path());
    }
}