#include "WorldCoordinate.h"

using namespace Math;

WorldCoordinate::WorldCoordinate(void) :
    scale_(1.f, 1.f, 1.f), position_(0.f, 0.f, 0.f), rotation_(Rotate{ false })
{
}

WorldCoordinate::WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot) :
    scale_(scale), position_(pos), rotation_(Rotate{ rot })
{
}

void WorldCoordinate::Update(void)
{
    Matrix4 matScale{ Matrix::Identity() };
    Matrix4 matRotate{ Matrix::Identity() };

    matScale = Matrix::Scale(scale_);

    if (rotation_.eular.x || rotation_.eular.y || rotation_.eular.z) {
        matRotate *= Matrix::RotationZ(rotation_.eular.z);
        matRotate *= Matrix::RotationX(rotation_.eular.x);
        matRotate *= Matrix::RotationY(rotation_.eular.y);
    }
    else {
        matRotate *= Math::QuaternionF::MakeRotateMatrix3(rotation_.axisQ.forward, rotation_.axisQ.right, rotation_.axisQ.up);
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
    if (rotation_.eular.x || rotation_.eular.y || rotation_.eular.z) {
        rotation_.eular = { 0.f,0.f,0.f };
    }
    else {
        rotation_.axisQ = Axis3Q();
    }

    matWorld_ = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
}

WorldCoordinate::Rotate::Rotate(bool isEular)
{
    if (isEular) {
        eular = Vector3(0.f, 0.f, 0.f);
    }
    else {
        axisQ = Axis3Q();
    }
}
