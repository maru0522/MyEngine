#include "Quaternion.h"
#include <cmath>

Quaternion Quaternion::Identity(void) const
{
    return Quaternion{ 0,0,0,1 };
}

Quaternion Quaternion::Conjugate() const
{
    return Quaternion{ -x,-y,-z,w };
}

float Quaternion::Norm() const
{
    return std::sqrtf(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::Normalize(void) const
{
    Quaternion tmp{ *this };
    float len{ Norm() };
    if (len != 0) {
        return tmp / len;
    }
    return tmp;
}

float Quaternion::Dot(const Quaternion& q) const
{
    return x * q.x + y * q.y + z * q.z + w * q.w;
}

Quaternion Quaternion::Inverse(void) const
{
    return Quaternion{ this->Conjugate() / (this->Norm() * this->Norm()) };
}

Quaternion Quaternion::operator+() const
{
    return Quaternion{ +x, +y, +z, +w };
}

Quaternion Quaternion::operator-() const
{
    return Quaternion{ -x, -y, -z, -w };
}

Quaternion& Quaternion::operator+=(const Quaternion& q)
{
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& q)
{
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
}

Quaternion& Quaternion::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

Quaternion& Quaternion::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    w /= s;
    return *this;
}

const Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
    Quaternion tmp{ q1 };
    return tmp += q2;
}

const Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
    Quaternion tmp{ q1 };
    return tmp -= q2;
}

const Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
    Quaternion q{};
    q.x = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    q.y = q1.y * q2.z - q1.z * q2.y + q2.w * q1.x + q1.w * q2.x;
    q.z = q1.z * q2.x - q1.x * q2.z + q2.w * q1.y + q1.w * q2.y;
    q.w = q1.x * q2.y - q1.y * q2.x + q2.w * q1.z + q1.w * q2.z;
    return q;
}

const Quaternion operator*(const Quaternion& q, float s)
{
    Quaternion tmp{ q };
    return tmp *= s;
}

const Quaternion operator/(const Quaternion& q, float s)
{
    Quaternion tmp{ q };
    return tmp /= s;
}

Quaternion Math::QuaternionF::MakeAxisAngle(const Vector3& axis, float radian)
{
    return Quaternion{ axis.x * sinf(radian / 2),
                       axis.y * sinf(radian / 2),
                       axis.z * sinf(radian / 2),
                       cosf(radian / 2) };
}

Quaternion Math::QuaternionF::EulerToQuaternion(const Vector3& eular)
{
    Quaternion qx = Math::QuaternionF::MakeAxisAngle({ 1,0,0 }, eular.x);
    Quaternion qy = Math::QuaternionF::MakeAxisAngle({ 0,1,0 }, eular.y);
    Quaternion qz = Math::QuaternionF::MakeAxisAngle({ 0,0,1 }, eular.z);

    return qx * qy * qz;
}

Vector3 Math::QuaternionF::RotateVector(const Vector3& v, const Quaternion& q)
{
    Quaternion tmp = q * Quaternion{ v.x,v.y,v.z,0 };
    Quaternion result = tmp * q.Conjugate();

    return Vector3(result.x, result.y, result.z);
}

Matrix4 Math::QuaternionF::MakeRotateMatrix(const Quaternion& q)
{
    Matrix4 mat{};

    mat.m[0][0] = q.x * q.x + q.y * q.y - q.z * q.z - q.w * q.w;
    mat.m[0][1] = 2 * (q.x * q.y + q.w * q.z);
    mat.m[0][2] = 2 * (q.x * q.z - q.w * q.y);
    mat.m[0][3] = 0;

    mat.m[1][0] = 2 * (q.x * q.y + q.w * q.z);
    mat.m[1][1] = q.x * q.x - q.y * q.y + q.z * q.z - q.w * q.w;
    mat.m[1][2] = 2 * (q.y * q.z + q.w * q.x);
    mat.m[1][3] = 0;

    mat.m[2][0] = 2 * (q.x * q.z + q.w * q.y);
    mat.m[2][1] = 2 * (q.y * q.z - q.w * q.x);
    mat.m[2][2] = q.x * q.x - q.y * q.y - q.z * q.z + q.w * q.w;
    mat.m[2][3] = 0;

    mat.m[3][0] = 0;
    mat.m[3][1] = 0;
    mat.m[3][2] = 0;
    mat.m[3][3] = 1;

    return mat;
}

Matrix4 Math::QuaternionF::MakeRotateMatrix3(const Quaternion& q1, const Quaternion& q2, const Quaternion& q3)
{
    Quaternion q = q1 * q2 * q3;
    Matrix4 mat{};

    mat.m[0][0] = q.x * q.x + q.y * q.y - q.z * q.z - q.w * q.w;
    mat.m[0][1] = 2 * (q.x * q.y + q.w * q.z);
    mat.m[0][2] = 2 * (q.x * q.z - q.w * q.y);
    mat.m[0][3] = 0;

    mat.m[1][0] = 2 * (q.x * q.y + q.w * q.z);
    mat.m[1][1] = q.x * q.x - q.y * q.y + q.z * q.z - q.w * q.w;
    mat.m[1][2] = 2 * (q.y * q.z + q.w * q.x);
    mat.m[1][3] = 0;

    mat.m[2][0] = 2 * (q.x * q.z + q.w * q.y);
    mat.m[2][1] = 2 * (q.y * q.z - q.w * q.x);
    mat.m[2][2] = q.x * q.x - q.y * q.y - q.z * q.z + q.w * q.w;
    mat.m[2][3] = 0;

    mat.m[3][0] = 0;
    mat.m[3][1] = 0;
    mat.m[3][2] = 0;
    mat.m[3][3] = 1;

    return mat;
}

Quaternion Math::QuaternionF::Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
    float dot{ q0.Dot(q1) };
    Quaternion lq0{ q0 };

    if (dot < 0) {
        lq0 = -lq0;
        dot = -dot;
    }

    if (dot >= 1.f - 0.0005f) {
        return lq0 * (1.f - t) + q1 * t;
    }

    float theta{ std::acos(dot) };

    return lq0 * (std::sin((1.f - t) * theta) / std::sin(theta)) + q1 * (std::sin(t * theta) / std::sin(theta));
}

Quaternion Math::QuaternionF::DirectionToDirection(const Vector3& u, const Vector3& v)
{
    Vector3 w{ u.cross(v) };
    Vector3 n = w.normalize();

    Vector3 uCopy{ u }, vCopy{ v };

    float theta{ std::acosf(Vector3{ uCopy.normalize() }.dot(vCopy.normalize())) };

    return { n.x * std::sinf(theta / 2.f), n.y * std::sinf(theta / 2.f), n.z * std::sinf(theta / 2.f), std::cosf(theta / 2.f) };
}
