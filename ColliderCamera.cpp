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
        // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定
        float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - (sphereCollider_.GetColInfo().f + sphereCollider_.radius);

        Vector3 currentPos = GetCoordinatePtr()->GetPosition();

        // 正規化された、星からカメラへのベクトル * めり込み距離
        currentPos += repelVec_ * -diff; // ここをマイナス符号で値反転

        GetCoordinatePtr()->SetPosition(currentPos);
    }
}
