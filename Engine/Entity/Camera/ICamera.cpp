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
    // アフィン変換計算に姿勢を使用するか（true = rotは使わなくなる）
    is_affinUseAxes_ = false;

    // 更新処理を一度挟む
    UpdateOrthoGraphic();

    screen_.SetMatViewPtr(&matView_);
    screen_.SetMatPrjPtr(&matProj_Perspective_);
}

void ICamera::Update(void)
{
    // アフィン変換計算にどちらを使用するか
    is_affinUseAxes_ ?
        // position, axes の情報からワールド行列を生成
        transformMatrix_.mat_world = Math::Function::AffinTrans(transform_, axes_, &transformMatrix_) :
        // position, rotation, scale の情報からワールド行列を生成
        transformMatrix_.mat_world = Math::Function::AffinTrans(transform_, &transformMatrix_);

    // 行列から各軸の向きを抽出し、それをカメラの向きとして適用
    axes_.forward = transformMatrix_.GetMatAxisZ();
    axes_.right = transformMatrix_.GetMatAxisX();
    axes_.up = transformMatrix_.GetMatAxisY();

    // ターゲットが設定されている場合
    if (vec_target_.IsNonZero())
    {
        // ターゲットへの方向を正面として設定
        axes_.forward = vec_target_ - transform_.position;
        // 正規化
        axes_.forward = axes_.forward.Normalize();
    }

    // ビュー行列
    matView_ = Math::Mat4::ViewLookToLH(transform_.position, axes_.forward, axes_.up);

    // 射影行列
    matProj_Perspective_ = Math::Mat4::ProjectionPerspectiveFovLH(Math::Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
}

void ICamera::UpdateOrthoGraphic(void)
{
    matProj_OrthoGraphic_ = Math::Mat4::ProjectionOrthoGraphicLH(WndAPI::kWidth_, WndAPI::kHeight_);
}
