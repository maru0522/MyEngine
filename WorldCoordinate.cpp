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
    axes_(forward, right, up)
{
}

void WorldCoordinate::Update(void)
{
    Matrix4 matScale{ Mat4::Identity() };
    Matrix4 matRotate{ Mat4::Identity() };

    matScale = Mat4::Scale(scale_);

    if (isPriorityEular_) {
        matRotate *= Mat4::RotationZ(eular_.z);
        matRotate *= Mat4::RotationX(eular_.x);
        matRotate *= Mat4::RotationY(eular_.y);
    }
    else {
        //Quaternion forward = Math::QuaternionF::MakeAxisAngle(axes_.forward.ExtractVector3().Normalize(), axes_.forward.w);
        //Quaternion right = Math::QuaternionF::MakeAxisAngle(axes_.right.ExtractVector3().Normalize(), axes_.right.w);
        //Quaternion up = Math::QuaternionF::MakeAxisAngle(axes_.up.ExtractVector3().Normalize(), axes_.up.w);
        //matRotate = Math::QuaternionF::MakeRotateMatrix3(forward, right, up);
        matRotate = Math::QuaternionF::MakeRotateMatrix(axes_.up);
    }

    matWorld_ = Mat4::Identity();
    matWorld_ *= matScale;
    matWorld_ *= matRotate;
    matWorld_ = Mat4::Translate(matWorld_, position_);
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
