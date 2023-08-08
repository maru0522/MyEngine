#pragma once
#include "Vector3.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

class ColliderCamera : public Camera
{
public:
    const float kColRadius_{ 3.f };

    // 関数
    ColliderCamera(CollisionManager* colMPtr);
    void Update(void) override;

private:
    void OnCollision(void);

    // 変数
    Vector3 repelVec_;
    CollisionPrimitive::SphereCollider sphereCollider_;

public:
    // setter
    void SetPlanetCenter(const Vector3& planetCenter) {
        // Updateに引数つけるのは嫌だけど、カプセル化するうえで、他のクラスに処理書くよりはマシ
        // 星1つに対して、ColliderCameraも1つの計算なので、コライダーのID指定でcenterを受け取るわけにはいかない。

        // 星からカメラ方向へのベクトル
        repelVec_ = (transform_.position - planetCenter).Normalize();
    }

    void SetViewMat(const Matrix4& viewMat) { matView_ = viewMat; }
};

