#pragma once
#include "Object3D.h"
#include "CollisionManager.h"

class StickyBall :
    public Object3D
{
public:
    // �萔
    const float kDistance_to_root_{ 1.f };      // ���{���畂���Ă鋅�܂ł̋���
    const float kDistance_to_defPos_{ 5.f };    // ���̋��̍��W����ǂꂾ������邱�Ƃ��ł��邩
    const float kRadius_{ 2.f };                // �����̂̑傫��

    // �֐�
    StickyBall(CollisionManager* colMPtr);
    ~StickyBall(void);

    void Update(void) override;
    void Draw(void) override;

private:
    void OnCollision(void);

    // �ϐ�
    Transform transform_; // �����̍��W
    Axis3 axes_;

    CollisionPrimitive::SphereCollider collision_sphere_; // ���̍��W
    CollisionManager* colMPtr_;
    float distance_to_defPos_;

public:
    // getter
    Transform* TransformPtr(void) { return &transform_; }

    // setter
    void SetRoot(const Vector3& pos);
};

