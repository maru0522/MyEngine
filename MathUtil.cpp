#include "MathUtil.h"

float Math::Ease::EaseInSine(float t, float start, float end)
{
    return start + (end - start) * (1.f - std::cosf(t * (Math::PI / 2.f)));
}

float Math::Ease::EaseInCubic(float t, float start, float end)
{
    return start + (end - start) * (t * t * t);
}

float Math::Ease::EaseInQuint(float t, float start, float end)
{
    return start + (end - start) * (t * t * t * t * t);
}

float Math::Ease::EaseInCirc(float t, float start, float end)
{
    float time{ 1.f - std::sqrtf(1.f - t * t) };
    return start * (1.f - time) + end * time;
}

float Math::Ease::EaseInElastic(float t, float start, float end)
{
    const float c4{ (2.f * Math::PI) / 3.f };
    if (t == 0.f) return start;
    else if (t == 1.f) return end;
    else return -std::powf(2.f, 10.f * t - 10.f) * std::sinf((t * 10.f - 10.75f) * c4);
}


float Math::Ease::EaseOutSine(float t, float start, float end)
{
    float time{ std::sinf((t * Math::PI) / 2.f) };
    return start + (end - start) * time;
}

float Math::Ease::EaseOutCubic(float t, float start, float end)
{
    float time{ 1.f - t * t * t };
    return start * (1.f - time) + end * time;
}

float Math::Ease::EaseOutQuint(float t, float start, float end)
{
    float value{ 1.f - t };
    return start + (end - start) * (1.f - value * value * value * value * value);
}

float Math::Ease::EaseOutCirc(float t, float start, float end)
{
    float value{ t - 1 };
    return start + (end - start) * std::sqrtf(1.f - value * value);
}

float Math::Ease::EaseOutElastic(float t, float start, float end)
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

float Math::Ease::EaseInOutSine(float t, float start, float end)
{
    float time{ -(std::cosf(Math::PI * t) - 1.f) / 2.f };
    return start * (1.f - time) + end * time;
}

float Math::Ease::EaseInOutCubic(float t, float start, float end)
{
    float time;
    if (t < 0.5f) {
        time = 4.f * t * t * t;
    }
    else {
        time = 1.f - powf(-2.f * t + 2.f, 3.f) / 2.f;
    }
    return start * (1.f - time) + end * time;
}

float Math::Ease::EaseInOutQuint(float t, float start, float end)
{
    float value{ -2.f * t + 2.f };
    return start + (end - start) * (t < 0.5f ? 16.f * t * t * t * t * t : 1.f - value * value * value * value * value / 2.f);
}

float Math::Ease::EaseInOutCirc(float t, float start, float end)
{
    return start + (t < 0.5f
        ? (1.f - std::sqrtf(1.f - std::powf(2.f * t, 2.f))) / 2.f
        : (std::sqrtf(1.f - std::powf(-2.f * t + 2.f, 2.f)) + 1.f) / 2.f) * (end - start);
}

float Math::Ease::EaseInOutElastic(float t, float start, float end)
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