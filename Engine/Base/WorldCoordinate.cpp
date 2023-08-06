#include "WorldCoordinate.h"
#include "Input.h"

using namespace Math;

//WorldCoordinate::WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot) :
//    position_(pos), scale_(scale), eular_(rot), isPriorityEular_(true),
//    axes_(Axis3{})
//{
//}
//
//WorldCoordinate::WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& forward, const Vector3& right, const Vector3& up) :
//    position_(pos), scale_(scale), eular_(Vector3{ 0.f,0.f,0.f }), isPriorityEular_(false),
//    axes_(forward, right, up)
//{
//}
//
//void WorldCoordinate::Update(void)
//{
//    Matrix4 matScale{ Mat4::Identity() };
//    Matrix4 matRotate{ Mat4::Identity() };
//
//    matScale = Mat4::Scale(scale_);
//
//    if (isPriorityEular_) {
//        matRotate *= Mat4::RotationZ(eular_.z);
//        matRotate *= Mat4::RotationX(eular_.x);
//        matRotate *= Mat4::RotationY(eular_.y);
//    }
//    else {
//        Vector3 forward = axes_.forward.Normalize();
//        Vector3 right = axes_.right.Normalize();
//        Vector3 up = axes_.up.Normalize();
//        matRotate = Math::Mat4::RotMatFromAxes3(forward, right, up);
//    }
//
//    matWorld_ = Mat4::Identity();
//    matWorld_ *= matScale;
//    matWorld_ *= matRotate;
//    matWorld_ = Mat4::Translate(matWorld_, position_);
//}
//
//void WorldCoordinate::Reset(void)
//{
//    scale_ = { 1.f, 1.f, 1.f };
//    position_ = { 0.f, 0.f, 0.f };
//    eular_ = { 0.f,0.f,0.f };
//    axes_ = Axis3();
//
//    matWorld_ = {
//        1.0f, 0.0f, 0.0f, 0.0f,
//        0.0f, 1.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 1.0f, 0.0f,
//        0.0f, 0.0f, 0.0f, 1.0f,
//    };
//}

WorldCoordinate2::WorldCoordinate2(const Matrix4& world, const Matrix4& sca, const Matrix4& rot, const Matrix4& pos)
    : mat_world(world), mat_sca(sca), mat_rot(rot), mat_pos(pos)
{
}

void WorldCoordinate2::CompositionSRT(void)
{
    mat_world *= mat_sca;
    mat_world *= mat_rot;
    mat_world *= mat_pos;
}

void WorldCoordinate2::Initialize(void)
{
    Matrix4 identity = Math::Mat4::Identity();

    mat_world = identity;
    mat_sca = identity;
    mat_rot = identity;
    mat_pos = identity;
}
