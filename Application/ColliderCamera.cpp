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
        // ビュー行列
        matView_ = Math::Mat4::Inverse(coordinate_.mat_world);

        // 射影行列
        matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
    }

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

void ColliderCamera::CalcAxis3(const Vector3& playerPos, const Vector3& pUpVec)
{
    axes_.forward = (playerPos - transform_.position).Normalize();

    // プレイヤーの上ベクトルとの外積でカメラの右向きベクトルを定義。
    // カメラ位置が、プレイヤー上ベクトルのラインより下なら、正しく右向きが出る。
    axes_.right = pUpVec.Cross(axes_.forward).Normalize();

    // 右向きベクトルが反転してると下向きベクトルになってしまうから注意。
    axes_.up = axes_.forward.Cross(axes_.right).Normalize();

    // なんでこれだとうまくいくんや？行列からの各軸抜き出しとの違いを出すべきかも。
    axes_.forward = coordinate_.GetMatAxisZ();
    axes_.right = coordinate_.GetMatAxisX();
    axes_.up = coordinate_.GetMatAxisY();
}
