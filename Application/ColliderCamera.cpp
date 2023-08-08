#include "ColliderCamera.h"
#include "MathUtil.h"
#include "WndAPI.h"

ColliderCamera::ColliderCamera(CollisionManager* colMPtr)
{
    colMPtr->Register(&sphereCollider_);
    sphereCollider_.SetID("colliderCamera");
    sphereCollider_.SetOnCollision(std::bind(&ColliderCamera::OnCollision, this));
    sphereCollider_.radius = kColRadius_;
    GetAxis3Ptr()->forward = { 0,0,-1 };
    GetAxis3Ptr()->right = { -1,0,0 };
}

void ColliderCamera::Update(void)
{
    using namespace Math;

    coordinate_.mat_world = Math::Function::AffinTrans(transform_);

    // ビュー行列
    //isFollow_ ?
    //    matView_ = Math::Mat4::ViewLookAtLH(coordinate_.GetPosition(), *targetPtr_, coordinate_.GetMatAxisY()) :
    //    matView_ = Math::Mat4::ViewLookToLH(coordinate_.GetPosition(), coordinate_.GetForwardVec().Normalize(), coordinate_.GetUpVec().Normalize());
    matView_ = Math::Mat4::Inverse(coordinate_.mat_world);

    // 射影行列
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);

    sphereCollider_.center = transform_.position;
}

void ColliderCamera::OnCollision(void)
{
    //if (sphereCollider_.GetColInfo().id == "repelCameraArea")
    //{
    //    // めり込み距離を出す (めり込んでいる想定 - 距離）なので結果はマイナス想定
    //    float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - (sphereCollider_.GetColInfo().f + sphereCollider_.radius);

    //    Vector3 currentPos = GetCoordinatePtr()->GetPosition();

    //    // 正規化された、星からカメラへのベクトル * めり込み距離
    //    currentPos += repelVec_ * -diff; // ここをマイナス符号で値反転

    //    GetCoordinatePtr()->SetPosition(currentPos);
    //}
}
