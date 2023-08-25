#pragma once
#include <string>
#include <chrono>
#include "FileSystem.h"
#include <typeinfo>

namespace Util {

    namespace Convert {
        std::string ToString(const std::wstring& wStr);
        std::string ToString(const char* cStr);
        std::wstring ToWString(const std::string& str);
        std::wstring ToWString(const char* cStr);

        // クラス名をstringで取得
        template<class T> inline std::string NameOf(bool removePrefix = false) {
            std::string str{ typeid(T).name() };

            if (removePrefix) {
                if (str.size() < 7)
                    return str;

                str[0] == 'c' ? str.erase(0, 6) : str.erase(0, 7);
                return str;
            }

            return str;
        }

        template<class T, size_t N> std::array<T, N> ToArray(const std::vector<T>& vec) {
            return std::array<T, vec.size()>{vec.begin(), vec.end()};
        }

        template<class T, size_t N> std::vector<T> ToVector(const std::array<T, N>& arr) {
            return std::vector<T>{arr.begin(), arr.end()};
        }
    }

    class Timer {
    public:
        // 定義
        using nanoseconds = std::chrono::nanoseconds;
        using microseconds = std::chrono::microseconds;
        using milliseconds = std::chrono::milliseconds;
        using seconds = std::chrono::seconds;
        using minutes = std::chrono::minutes;
        using hours = std::chrono::hours;

        // 静的関数
        /// <summary>
        /// <para>Windowsが起動してからの経過時間をミリ秒単位であらわした値を返す。</para>
        /// <para>日時等を取得するわけではないが、この関数の存在意義は時間計測にある。</para>
        /// <para>TimeUnitはstd::chrono::durationの別名にある[時間間隔を表す型]</para>
        /// <para>のみを使用できる。</para>
        /// </summary>
        /// <typeparam name="TimeUnit"></typeparam>
        /// <returns></returns>
        template<class TimeUnit>
        static int32_t GetNowCount(void);
        static std::string GetNowDateTime(void);

        // 関数
        void Start(float endTime = 1.0f);

        inline void SetEndTime(float endTime) { endTime_ = endTime; }

        inline const float GetEndTime(void) { return endTime_; }
        inline const bool GetIsEnd(void) { return endTime_ < ((GetNowCount<milliseconds>() - startTime_) / 1000.0f); } // endTime < elapsedTime
        inline const float GetElapsedTime(void) { return float{ (GetNowCount<milliseconds>() - startTime_) / 1000.0f }; }

    private:
        // 変数
        int32_t startTime_{ 0 }; // 計測開始時間
        float endTime_{ 0.f }; // 到達目標時間
    };

    namespace Log {
        void PrintOutputWindow(const std::string& str);
        void PrintExternalText(const std::string& str, const std::experimental::filesystem::path& logTextPath = std::experimental::filesystem::current_path());
    }
}