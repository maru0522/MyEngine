#include "ColliderCamera.h"
#include "MathUtil.h"
#include "WndAPI.h"

ColliderCamera::ColliderCamera(void)
{
}

ColliderCamera::~ColliderCamera(void)
{
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
