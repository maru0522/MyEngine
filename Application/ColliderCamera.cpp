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

    if (is_oldUpdateMethod_)
    {
        Camera::Update();
    }
    else
    {
        // �r���[�s��
        matView_ = Math::Mat4::Inverse(coordinate_.mat_world);

        // �ˉe�s��
        matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
    }

    sphereCollider_.center = transform_.position;
}

void ColliderCamera::OnCollision(void)
{
    //if (sphereCollider_.GetColInfo().id == "repelCameraArea")
    //{
    //    // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j�Ȃ̂Ō��ʂ̓}�C�i�X�z��
    //    float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - (sphereCollider_.GetColInfo().f + sphereCollider_.radius);

    //    Vector3 currentPos = GetCoordinatePtr()->GetPosition();

    //    // ���K�����ꂽ�A������J�����ւ̃x�N�g�� * �߂荞�݋���
    //    currentPos += repelVec_ * -diff; // �������}�C�i�X�����Œl���]

    //    GetCoordinatePtr()->SetPosition(currentPos);
    //}
}

void ColliderCamera::CalcAxis3(const Vector3& playerPos, const Vector3& pUpVec)
{
    axes_.forward = (playerPos - transform_.position).Normalize();

    // �v���C���[�̏�x�N�g���Ƃ̊O�ςŃJ�����̉E�����x�N�g�����`�B
    // �J�����ʒu���A�v���C���[��x�N�g���̃��C����艺�Ȃ�A�������E�������o��B
    axes_.right = pUpVec.Cross(axes_.forward).Normalize();

    // �E�����x�N�g�������]���Ă�Ɖ������x�N�g���ɂȂ��Ă��܂����璍�ӁB
    axes_.up = axes_.forward.Cross(axes_.right).Normalize();

    // �Ȃ�ł��ꂾ�Ƃ��܂��������H�s�񂩂�̊e�������o���Ƃ̈Ⴂ���o���ׂ������B
    axes_.forward = coordinate_.GetMatAxisZ();
    axes_.right = coordinate_.GetMatAxisX();
    axes_.up = coordinate_.GetMatAxisY();
}
