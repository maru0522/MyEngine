#include "WorldCoordinate.h"

using namespace Math;

WorldCoordinate::WorldCoordinate(const Vector3& pos, const Vector3& scale, const Vector3& rot) : 
    position_(pos), scale_(scale), rotation_(rot)
{
}

void WorldCoordinate::Update(void)
{
    Matrix4 matScale{ Matrix::Identity() };
    Matrix4 matTrans{ Matrix::Identity() };
    Matrix4 matRotate{ Matrix::Identity() };

    matScale = Matrix::Scale(scale_);

    matTrans = Matrix::Translate(position_);

    matRotate = Matrix::RotateZ(rotation_.z);
    matRotate = Matrix::RotateX(rotation_.x);
    matRotate = Matrix::RotateY(rotation_.y);

    matWorld_ = Matrix::Identity();
    matWorld_ = matScale;
    matWorld_ = matRotate;
    matWorld_ = matTrans;
}
