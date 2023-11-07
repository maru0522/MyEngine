#include "SphericalCamera.h"
#include "MathUtil.h"
#include "WndAPI.h"
#include "SimplifyImGui.h"
#include "Quaternion.h"

SphericalCamera::SphericalCamera(void)
{
    theta_ = 0.f;
    phi_ = 0.f;
}

SphericalCamera::~SphericalCamera(void)
{
}

void SphericalCamera::Update(void)
{
    using namespace Math;


    // ビュー行列
    matView_ = Math::Mat4::Inverse(coordinate_.mat_world);

    // 射影行列
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);

    //## 球面座標処理込みの行列生成
    Matrix4 matWorld = Mat4::Identity();
    Matrix4 matRotate = Mat4::Identity();

    // プレイヤーとの距離だけ、球面座標の中心点から下がらせる
    matWorld *= Mat4::Translate(matWorld, { 0, 0, -radius_ });
    // 球面座標系の座標仕様に合わせて回転（移動）させる
    matRotate = Mat4::RotationZ(psi_) * Mat4::RotationX(theta_) * Mat4::RotationY(phi_);
    matWorld *= matRotate;
    // ワールド座標の位置に持っていく（原点での計算後、プレイヤーの座標へ移動）
    matWorld.m[3][0] += transform_.position.x;
    matWorld.m[3][1] += transform_.position.y;
    matWorld.m[3][2] += transform_.position.z;

    coordinate_.mat_world = matWorld;

    transform_.position = { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] };
    //##

    // 姿勢の軸方向をそのまま 3つの軸から適用
    axes_.forward = coordinate_.GetMatAxisZ().Normalize();
    axes_.right = coordinate_.GetMatAxisX().Normalize();
    axes_.up = coordinate_.GetMatAxisY().Normalize();

}

//void SphericalCamera::CameraBehavior(Behavior arg_camerawork)
//{
//    using namespace Math;
//
//    if (arg_camerawork == Behavior::A) {}
//    //{
//    //    // 視線方向
//    //    Vector3 vec_eyeDirection = Vector3(pos_target - transform_.position).Normalize();
//
//    //    // ビュー行列
//    //    isFollow_ ?
//    //        matView_ = Math::Mat4::ViewLookAtLH(transform_.position, *targetPtr_, coordinate_.GetMatAxisY()) :
//    //        matView_ = Math::Mat4::ViewLookToLH(transform_.position, vec_eyeDirection, axes_.up);
//
//    //    // 射影行列
//    //    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
//
//    //    //## 球面座標処理込みの行列生成
//    //    Matrix4 matWorld = Mat4::Identity();
//    //    Matrix4 matRotate = Mat4::Identity();
//
//    //    // プレイヤーとの距離だけ、球面座標の中心点から下がらせる
//    //    matWorld *= Mat4::Translate(matWorld, { 0, 0, -radius_ });
//    //    // 球面座標系の座標仕様に合わせて回転（移動）させる
//    //    matRotate = Mat4::RotationX(theta_) * Mat4::RotationY(phi_);
//    //    matWorld *= matRotate;
//    //    // ワールド座標の位置に持っていく（原点での計算後、プレイヤーの座標へ移動）
//    //    matWorld.m[3][0] += transform_.position.x;
//    //    matWorld.m[3][1] += transform_.position.y;
//    //    matWorld.m[3][2] += transform_.position.z;
//
//    //    coordinate_.mat_world = matWorld;
//    //    transform_.position = { matWorld.m[3][0],matWorld.m[3][1],matWorld.m[3][2] };
//    //    //##
//
//    //    // カメラが見ている方向を正面ベクトルに設定
//    //    axes_.forward = vec_eyeDirection;
//    //    // プレイヤーの上ベクトルと、カメラの正面ベクトルから、カメラの右ベクトルを計算
//    //    axes_.right = vec_playerUp_.Cross(vec_eyeDirection).Normalize();
//    //    // カメラの正面ベクトルと、カメラの右ベクトルから、カメラの上ベクトルを計算
//    //    axes_.up = vec_eyeDirection.Cross(axes_.right).Normalize();
//    //}
//    //else if(arg_camerawork == Behavior::B)
//    {
//
//    }
//}

void SphericalCamera::SetSphericalRotate(float arg_theta, float arg_phi, float arg_psi)
{
    theta_ = arg_theta;
    phi_ = arg_phi;
    psi_ = arg_psi;
}

void SphericalCamera::CalcAxis3(const Vector3& playerPos, const Vector3& pUpVec)
{
    vec_playerUp_ = pUpVec;

    axes_.forward = (playerPos - transform_.position).Normalize();

    // プレイヤーの上ベクトルとの外積でカメラの右向きベクトルを定義。
    // カメラ位置が、プレイヤー上ベクトルのラインより下なら、正しく右向きが出る。
    axes_.right = pUpVec.Cross(axes_.forward).Normalize();

    // 右向きベクトルが反転してると下向きベクトルになってしまうから注意。
    axes_.up = axes_.forward.Cross(axes_.right).Normalize();

    //// なんでこれだとうまくいくんや？行列からの各軸抜き出しとの違いを出すべきかも。
    //axes_.forward = coordinate_.GetMatAxisZ();
    //axes_.right = coordinate_.GetMatAxisX();
    //axes_.up = coordinate_.GetMatAxisY();
}

void SphericalCamera::Debug_need(float arg_rad, const Vector3& arg_sphericalRotate, const Vector3& pos, const Vector3& pos_eye)
{
    radius_ = arg_rad;
    theta_ = arg_sphericalRotate.x;
    phi_ = arg_sphericalRotate.y;
    psi_ = arg_sphericalRotate.z;

    transform_.position = pos;
    pos_target = pos_eye;
}

void SphericalCamera::Debug_need(float arg_rad, const Vector3& pos, const Vector3& pos_eye)
{
    radius_ = arg_rad;
    transform_.position = pos;
    pos_target = pos_eye;
}

void SphericalCamera::Debug_need2(float arg_psi)
{
    psi_ = arg_psi;
}
