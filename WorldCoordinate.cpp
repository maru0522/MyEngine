#include "WorldCoordinate.h"
#include "Input.h"

using namespace Math;

WorldCoordinate::WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot) :
    position_(pos), scale_(scale), eular_(rot), isPriorityEular_(true),
    axes_(Axis3{})
{
}

WorldCoordinate::WorldCoordinate(const Vector3& pos, const Vector3& scale, const Quaternion& forward, const Quaternion& right, const Quaternion& up) :
    position_(pos), scale_(scale), eular_(Vector3{ 0.f,0.f,0.f }), isPriorityEular_(false),
    axes_(forward,right,up)
{
}

void WorldCoordinate::Update(void)
{
    Matrix4 matScale{ Matrix::Identity() };
    Matrix4 matRotate{ Matrix::Identity() };

    matScale = Matrix::Scale(scale_);

    static float radx = 0;
    static float rady = 0;
    static float radz = 0;
    if (KEYS::IsDown(DIK_NUMPAD9)) radz += 0.01f;
    if (KEYS::IsDown(DIK_NUMPAD6)) radx += 0.01f;
    if (KEYS::IsDown(DIK_NUMPAD3)) rady += 0.01f;

    if (isPriorityEular_) {
        matRotate *= Matrix::RotationZ(eular_.z);
        matRotate *= Matrix::RotationX(eular_.x);
        matRotate *= Matrix::RotationY(eular_.y);
    }
    else {
        //if (KEYS::IsDown(DIK_K))
        //    quaternions_.forward = Math::QuaternionF::DirectionToDirection({ 0,0,1 }, Vector3(1,0,0).normalize());
        //matRotate *= Math::QuaternionF::MakeRotateMatrix3(quaternions_.forward, quaternions_.right, quaternions_.up);
        axes_.up = Math::QuaternionF::MakeAxisAngle(Vector3(0, 1, 0).normalize(), rady);
        matRotate = Math::QuaternionF::MakeRotateMatrix(axes_.up);
    }

    matWorld_ = Matrix::Identity();
    matWorld_ *= matScale;
    matWorld_ *= matRotate;
    matWorld_ = Matrix::Translate(matWorld_, position_);
}

void WorldCoordinate::Reset(void)
{
    scale_ = { 1.f, 1.f, 1.f };
    position_ = { 0.f, 0.f, 0.f };
    eular_ = { 0.f,0.f,0.f };
    axes_ = Axis3();

    matWorld_ = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
}
