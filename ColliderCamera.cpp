#include "ColliderCamera.h"


ColliderCamera::ColliderCamera(CollisionManager* colMPtr)
{
    colMPtr->Register(&sphereCollider_);
    sphereCollider_.SetID("colliderCamera");
    sphereCollider_.SetOnCollision(std::bind(&ColliderCamera::OnCollision, this));
    sphereCollider_.radius = kColRadius_;
}

void ColliderCamera::Update(void)
{
    Camera::Update();
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
