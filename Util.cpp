#include "Util.h"
#include <chrono>
#include <cmath>
#include <Windows.h>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>

float Util::Ease::EaseInSine(float t, float start, float end)
{
    return start + (end - start) * (1.f - std::cosf(t * (Math::PI / 2.f)));
}

float Util::Ease::EaseInCubic(float t, float start, float end)
{
    return start + (end - start) * (t * t * t);
}

float Util::Ease::EaseInQuint(float t, float start, float end)
{
    return start + (end - start) * (t * t * t * t * t);
}

float Util::Ease::EaseInCirc(float t, float start, float end)
{
    float time{ 1.f - std::sqrtf(1.f - t * t) };
    return start * (1.f - time) + end * time;
}

float Util::Ease::EaseInElastic(float t, float start, float end)
{
    const float c4{ (2.f * Math::PI) / 3.f };
    if (t == 0.f) return start;
    else if (t == 1.f) return end;
    else return -std::powf(2.f, 10.f * t - 10.f) * std::sinf((t * 10.f - 10.75f) * c4);
}


float Util::Ease::EaseOutSine(float t, float start, float end)
{
    float time{ std::sinf((t * Math::PI) / 2.f) };
    return start + (end - start) * time;
}

float Util::Ease::EaseOutCubic(float t, float start, float end)
{
    float value{ 1.f - t };
    float time{ 1.f - t * t * t };
    return start * (1.f - time) + end * time;
}

float Util::Ease::EaseOutQuint(float t, float start, float end)
{
    float value{ 1.f - t };
    return start + (end - start) * (1.f - value * value * value * value * value);
}

float Util::Ease::EaseOutCirc(float t, float start, float end)
{
    float value{ t - 1 };
    return start + (end - start) * std::sqrtf(1.f - value * value);
}

float Util::Ease::EaseOutElastic(float t, float start, float end)
{
    const float c4 = (2.f * Math::PI) / 3.f;

    if (t == 0) {
        return start;
    }
    else if (t == 1) {
        return end;
    }
    else {
        return start + (end - start) * (1.f - std::powf(2.f, -10 * t) * std::sinf((t * 10.f - 0.75f) * c4));
    }
}

float Util::Ease::EaseInOutSine(float t, float start, float end)
{
    float time{ -(std::cosf(Math::PI * t) - 1.f) / 2.f };
    return start * (1.f - time) + end * time;
}

float Util::Ease::EaseInOutCubic(float t, float start, float end)
{
    float value{ -2.f * t + 2.f };
    return t < 0.5f ? 4.f * t * t * t : 1.f - value * value * value / 2.f;
}

float Util::Ease::EaseInOutQuint(float t, float start, float end)
{
    float value{ -2.f * t + 2.f };
    return start + (end - start) * (t < 0.5f ? 16.f * t * t * t * t * t : 1.f - value * value * value * value * value / 2.f);
}

float Util::Ease::EaseInOutCirc(float t, float start, float end)
{
    return start + (t < 0.5f
        ? (1.f - std::sqrtf(1.f - std::powf(2.f * t, 2.f))) / 2.f
        : (std::sqrtf(1.f - std::powf(-2.f * t + 2.f, 2.f)) + 1.f) / 2.f) * (end - start);
}

float Util::Ease::EaseInOutElastic(float t, float start, float end)
{
    const float c5 = (2.f * Math::PI) / 4.5f;

    return start + (t == 0.f
        ? 0.f
        : t == 1.f
        ? (end - start)
        : t < 0.5f
        ? -(std::powf(2.f, 20.f * t - 10.f) * std::sinf((20.f * t - 11.125f) * c5)) / 2.f
        : (std::powf(2.f, -20.f * t + 10.f) * std::sinf((20.f * t - 11.125f) * c5)) / 2.f + 1.f) * (end - start);
}

using namespace std::chrono;

template<class TimeUnit>
inline int32_t Util::Timer::GetNowCount(void)
{
    // templateの型は[時間間隔をを表す型]でなければならない。
    // ref: https://cpprefjp.github.io/reference/chrono/duration_aliases.html

    return (int32_t)duration_cast<TimeUnit>(steady_clock::now().time_since_epoch()).count();
}

std::string Util::Timer::GetNowDateTime(void)
{
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &now);

    std::stringstream ss;
    ss << 1900 + tm.tm_year << ":" << std::setfill('0') << std::setw(2) << tm.tm_mon + 1 << ":" << tm.tm_mday << " "
        << std::setfill('0') << std::setw(2) << tm.tm_hour << ":" << std::setfill('0') << std::setw(2) << tm.tm_min
        << ":" << std::setfill('0') << std::setw(2) << tm.tm_sec;
    return ss.str();
}

void Util::Timer::Start(float endTime)
{
    startTime_ = GetNowCount<milliseconds>();
    endTime_ = endTime;
}

void Util::Log::PrintOutputWindow(const std::string& str)
{
    std::wstring wStr{ str.begin(),str.end() };

    OutputDebugStringW(wStr.c_str());
}

void Util::Log::PrintExternalText(const std::string& str, const std::experimental::filesystem::path& logTextPath)
{
    const std::string fileName{ "mLogs.txt" };
    std::ifstream ifs{ logTextPath };
    std::ofstream ofs{};

    if (ifs.is_open()) { // ログテキストを開けた場合
        ofs.open(fileName, std::ios::app); // ログテキスト展開
        std::string writeString{ Timer::GetNowDateTime() }; // 現在の日時を文字列型で取得
        ofs << writeString << " - " << str << std::endl;
        ofs.close();
    }
    else { // ログテキストを開けなかった場合
        ofs.open(fileName, std::ios::out); // ログテキスト生成
        std::string createDate{ Timer::GetNowDateTime() }; // 作成日時を文字列型で取得
        ofs << "This text log was created on " << createDate << std::endl;
        std::string writeString{ Timer::GetNowDateTime() }; // 現在の日時を文字列型で取得
        ofs << writeString << " - " << str << std::endl;
        ofs.close();
    }
}

std::string Util::Convert::ToString(const std::wstring& wStr)
{
    int32_t necessarySize{ WideCharToMultiByte(CP_UTF8, 0, &wStr[0], (int32_t)wStr.size(), NULL, 0, NULL, NULL) };
    std::string toStr(necessarySize, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wStr[0], (int32_t)wStr.size(), &toStr[0], necessarySize, NULL, NULL);
    return toStr;
}

std::string Util::Convert::ToString(const char* cStr)
{
    int32_t necessarySize{ MultiByteToWideChar(CP_UTF8, 0, cStr, -1, NULL, 0) };
    std::wstring wStr(necessarySize, 0);
    MultiByteToWideChar(CP_UTF8, 0, cStr, -1, &wStr[0], necessarySize);
    int32_t utf8Size{ WideCharToMultiByte(CP_UTF8, 0, &wStr[0], -1, NULL, 0, NULL, NULL) };
    std::string toStr(utf8Size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wStr[0], -1, &toStr[0], utf8Size, NULL, NULL);
    return toStr;
}


std::wstring Util::Convert::ToWString(const std::string& str)
{
    int32_t necessarySize{ MultiByteToWideChar(CP_UTF8, 0, &str[0], (int32_t)str.size(), NULL, 0) };
    std::wstring toWStr(necessarySize, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int32_t)str.size(), &toWStr[0], necessarySize);
    return toWStr;
}

std::wstring Util::Convert::ToWString(const char* cStr)
{
    int32_t necessarySize{ MultiByteToWideChar(CP_UTF8, 0, cStr, -1, NULL, 0) };
    std::wstring toWStr(necessarySize, 0);
    MultiByteToWideChar(CP_UTF8, 0, cStr, -1, &toWStr[0], necessarySize);
    return toWStr;
}


