#include "WorldCoordinate.h"

using namespace Math;

WorldCoordinate::WorldCoordinate(void) :
    scale_(1.f, 1.f, 1.f), position_(0.f, 0.f, 0.f), rotation_(0.f, 0.f, 0.f)
{
}

WorldCoordinate::WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot) :
    position_(pos), scale_(scale), rotation_(rot)
{
}

void WorldCoordinate::Update(void)
{
    Matrix4 matScale{ Matrix::Identity() };
    Matrix4 matRotate{ Matrix::Identity() };

    matScale = Matrix::Scale(scale_);

    matRotate *= Matrix::RotationZ(rotation_.z);
    matRotate *= Matrix::RotationX(rotation_.x);
    matRotate *= Matrix::RotationY(rotation_.y);

    matWorld_ = Matrix::Identity();
    matWorld_ *= matScale;
    matWorld_ *= matRotate;
    matWorld_ = Matrix::Translate(matWorld_, position_);
}
