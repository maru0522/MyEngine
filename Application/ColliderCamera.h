#pragma once
#include "Vector3.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

class ColliderCamera : public Camera
{
public:
    const float kColRadius_{ 3.f };

    // �֐�
    ColliderCamera(CollisionManager* colMPtr);
    void Update(void) override;

private:
    void OnCollision(void);

    // �ϐ�
    Vector3 repelVec_;
    CollisionPrimitive::SphereCollider sphereCollider_;

public:
    // setter
    void SetPlanetCenter(const Vector3& planetCenter) {
        // Update�Ɉ�������̂͌������ǁA�J�v�Z�������邤���ŁA���̃N���X�ɏ����������̓}�V
        // ��1�ɑ΂��āAColliderCamera��1�̌v�Z�Ȃ̂ŁA�R���C�_�[��ID�w���center���󂯎��킯�ɂ͂����Ȃ��B

        // ������J���������ւ̃x�N�g��
        repelVec_ = (transform_.position - planetCenter).Normalize();
    }

    void SetViewMat(const Matrix4& viewMat) { matView_ = viewMat; }
};

