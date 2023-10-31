#include "SphericalCamera.h"
#include "MathUtil.h"
#include "WndAPI.h"
#include "SimplifyImGui.h"

SphericalCamera::SphericalCamera(Player* playerPtr)
{
    theta_ = 0.f;
    phi_ = 0.f;
    playerPtr_ = playerPtr;
}

SphericalCamera::~SphericalCamera(void)
{
}

void SphericalCamera::Update(void)
{
    using namespace Math;

    {
        //// ビュー行列
        //matView_ = Math::Mat4::Inverse(coordinate_.mat_world);

        //// 射影行列
        //matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);

        // position, rotation, scale の情報のみからワールド行列を生成
        //coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);

        // ビュー行列
        isFollow_ ?
            matView_ = Math::Mat4::ViewLookAtLH(transform_.position, *targetPtr_, coordinate_.GetMatAxisY()) :
            matView_ = Math::Mat4::ViewLookToLH(transform_.position, pos_eyeDirection_, axes_.up);

        // 射影行列
        matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
    }


    Matrix4 matWorld = Mat4::Identity();
    Matrix4 matRotate = Mat4::Identity();

    // プレイヤーとの距離だけ、球面座標の中心点から下がらせる
    matWorld *= Mat4::Translate(matWorld, { 0, 0, -radius_ });
    // 球面座標系の座標仕様に合わせて回転（移動）させる
    matRotate = Mat4::RotationX(theta_) * Mat4::RotationY(phi_);
    matWorld *= matRotate;
    // ワールド座標の位置に持っていく（原点での計算後、プレイヤーの座標へ移動）
    matWorld.m[3][0] += transform_.position.x;
    matWorld.m[3][1] += transform_.position.y;
    matWorld.m[3][2] += transform_.position.z;

    coordinate_.mat_world = matWorld;

    axes_.forward = pos_eyeDirection_;
    axes_.right = playerPtr_->GetAxis3Ptr()->up.Cross(pos_eyeDirection_).Normalize();
    axes_.up = pos_eyeDirection_.Cross(axes_.right).Normalize();

    //// なんでこれだとうまくいくんや？行列からの各軸抜き出しとの違いを出すべきかも。
    //axes_.forward = coordinate_.GetMatAxisZ().Normalize();
    //axes_.right = coordinate_.GetMatAxisX().Normalize();
    //axes_.up = coordinate_.GetMatAxisY().Normalize();
}

void SphericalCamera::CalcAxis3(const Vector3& playerPos, const Vector3& pUpVec)
{
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

void SphericalCamera::Debug_need(const Vector3& arg_spherical, const Vector3& pos, const Vector3& pos_eye)
{
    radius_ = arg_spherical.x;
    theta_ = arg_spherical.y;
    phi_ = arg_spherical.z;

    transform_.position = pos;
    pos_eyeDirection_ = (pos_eye - transform_.position).Normalize();
}
