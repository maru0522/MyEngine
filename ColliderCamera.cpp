#include "ColliderCamera.h"
#include "MathUtil.h"
#include "WndAPI.h"

ColliderCamera::ColliderCamera(CollisionManager* colMPtr)
{
    colMPtr->Register(&sphereCollider_);
    sphereCollider_.SetID("colliderCamera");
    sphereCollider_.SetOnCollision(std::bind(&ColliderCamera::OnCollision, this));
    sphereCollider_.radius = kColRadius_;
}

void ColliderCamera::Update(void)
{
    using namespace Math;

    coordinate_.Update();

    // �r���[�s��
    //isFollow_ ?
    //    matView_ = Math::Mat4::ViewLookAtLH(coordinate_.GetPosition(), *targetPtr_, coordinate_.GetMatAxisY()) :
    //    matView_ = Math::Mat4::ViewLookToLH(coordinate_.GetPosition(), coordinate_.GetForwardVec().Normalize(), coordinate_.GetUpVec().Normalize());
    matView_ = Math::Mat4::Inverse(coordinate_.GetMatWorld());

    // �ˉe�s��
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);

    sphereCollider_.center = GetCoordinatePtr()->GetPosition();
}

void ColliderCamera::OnCollision(void)
{
    if (sphereCollider_.GetColInfo().id == "repelCameraArea")
    {
        // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j�Ȃ̂Ō��ʂ̓}�C�i�X�z��
        float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - (sphereCollider_.GetColInfo().f + sphereCollider_.radius);

        Vector3 currentPos = GetCoordinatePtr()->GetPosition();

        // ���K�����ꂽ�A������J�����ւ̃x�N�g�� * �߂荞�݋���
        currentPos += repelVec_ * -diff; // �������}�C�i�X�����Œl���]

        GetCoordinatePtr()->SetPosition(currentPos);
    }
}