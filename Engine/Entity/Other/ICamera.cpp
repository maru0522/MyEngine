#include "ICamera.h"
#include "MathUtil.h"
#include "WndAPI.h"

ICamera::ICamera(const std::string& arg_id)
{
    // id
    id_ = arg_id;

    // 投影の、始端と終端
    nearZ_ = kDef_nearZ_;
    farZ_ = kDef_farZ_;

    // 姿勢
    axes_.Initialize();
    // 座標
    transform_.Initialize();

    UpdateOrthoGraphic();
}

void ICamera::Update(void)
{
    // position, rotation, scale の情報のみからワールド行列を生成
    transformMatrix.mat_world = Math::Function::AffinTrans(transform_);

    // 行列から各軸の向きを抽出し、それをカメラの向きとして適用
    axes_.forward = transformMatrix.GetMatAxisZ();
    axes_.right = transformMatrix.GetMatAxisX();
    axes_.up = transformMatrix.GetMatAxisY();

    // ビュー行列
    matView_ = Math::Mat4::ViewLookToLH(transform_.position, axes_.forward, axes_.up);

    // 射影行列
    matProj_Perspective_ = Math::Mat4::ProjectionPerspectiveFovLH(Math::Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
}

void ICamera::UpdateOrthoGraphic(void)
{
    matProj_OrthoGraphic_ = Math::Mat4::ProjectionOrthoGraphicLH(WndAPI::kWidth_, WndAPI::kHeight_);
}
