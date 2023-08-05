#include "MathUtil.h"

float Math::Ease::EaseInSine(float t, float start, float end)
{
    return start + (end - start) * (1.f - std::cosf(t * (Math::kPI / 2.f)));
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
    const float c4{ (2.f * Math::kPI) / 3.f };
    if (t == 0.f) return start;
    else if (t == 1.f) return end;
    else return -std::powf(2.f, 10.f * t - 10.f) * std::sinf((t * 10.f - 10.75f) * c4);
}


float Math::Ease::EaseOutSine(float t, float start, float end)
{
    float time{ std::sinf((t * Math::kPI) / 2.f) };
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
    const float c4 = (2.f * Math::kPI) / 3.f;

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
    float time{ -(std::cosf(Math::kPI * t) - 1.f) / 2.f };
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
    const float c5 = (2.f * Math::kPI) / 4.5f;

    return start + (t == 0.f
        ? 0.f
        : t == 1.f
        ? (end - start)
        : t < 0.5f
        ? -(std::powf(2.f, 20.f * t - 10.f) * std::sinf((20.f * t - 11.125f) * c5)) / 2.f
        : (std::powf(2.f, -20.f * t + 10.f) * std::sinf((20.f * t - 11.125f) * c5)) / 2.f + 1.f) * (end - start);
}

//Vector3 Math::Function::ToSphericalCoordinate(const Vector3& rectAngularCoordiante)
//{
//    const Vector3& rac = rectAngularCoordiante;
//
//    // result = x:動径r, y:緯度θ, z:経度φ
//    Vector3 result;
//    result.x = std::sqrtf(rac.x * rac.x + rac.y * rac.y + rac.z * rac.z);
//    result.y = std::acosf(rac.z / result.x);
//    result.z = Function::SgnA<float>(rac.y) * std::acosf(rac.x / std::sqrtf(rac.x * rac.x + rac.y * rac.y));
//
//    return result;
//}

Vector3 Math::Function::ToCartesian(float r, float theta, float phi)
{
    // r:動径r, theta:緯度θ, phi:経度φ
    Vector3 result;

    result.x = r * std::sinf(theta) * std::cosf(phi);
    result.y = r * std::sinf(theta) * std::sinf(phi);
    result.z = r * std::cosf(theta);

    return result;
}

Matrix4 Math::Function::AffinTrans(const Transform& transform)
{
    Matrix4 matWorld{ Mat4::Identity() };
    Matrix4 matScale{ Mat4::Identity() };
    Matrix4 matRotate{ Mat4::Identity() };

    matScale = Mat4::Scale(transform.scale);

    matRotate *= Mat4::RotationZ(transform.rotation.z);
    matRotate *= Mat4::RotationX(transform.rotation.x);
    matRotate *= Mat4::RotationY(transform.rotation.y);

    matWorld *= matScale;
    matWorld *= matRotate;
    matWorld = Mat4::Translate(matWorld, transform.position);

    return matWorld;
}

Matrix4 Math::Function::AffinTrans(const Transform& transform, const Axis3& axes)
{
    Matrix4 matWorld{ Mat4::Identity() };
    Matrix4 matScale{ Mat4::Identity() };
    Matrix4 matRotate{ Mat4::Identity() };

    matScale = Mat4::Scale(transform.scale);

    Vector3 forward = axes.forward.Normalize();
    Vector3 right = axes.right.Normalize();
    Vector3 up = axes.up.Normalize();
    matRotate = Math::Mat4::RotMatFromAxes3(forward, right, up);

    matWorld *= matScale;
    matWorld *= matRotate;
    matWorld = Mat4::Translate(matWorld, transform.position);

    return matWorld;
}

Matrix4 Math::Function::AffinTrans(const Vector3& pos, const Vector3& scale, const Vector3& rotEular)
{
    Matrix4 matWorld{ Mat4::Identity() };
    Matrix4 matScale{ Mat4::Identity() };
    Matrix4 matRotate{ Mat4::Identity() };

    matScale = Mat4::Scale(scale);

    matRotate *= Mat4::RotationZ(rotEular.z);
    matRotate *= Mat4::RotationX(rotEular.x);
    matRotate *= Mat4::RotationY(rotEular.y);

    matWorld *= matScale;
    matWorld *= matRotate;
    matWorld = Mat4::Translate(matWorld, pos);

    return matWorld;
}

Matrix4 Math::Function::AffinTrans(const Vector3& pos, const Vector3& scale, const Axis3& axes)
{
    Matrix4 matWorld{ Mat4::Identity() };
    Matrix4 matScale{ Mat4::Identity() };
    Matrix4 matRotate{ Mat4::Identity() };

    matScale = Mat4::Scale(scale);

    Vector3 forward = axes.forward.Normalize();
    Vector3 right = axes.right.Normalize();
    Vector3 up = axes.up.Normalize();
    matRotate = Math::Mat4::RotMatFromAxes3(forward, right, up);

    matWorld *= matScale;
    matWorld *= matRotate;
    matWorld = Mat4::Translate(matWorld, pos);

    return matWorld;
}
