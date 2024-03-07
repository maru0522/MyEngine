#include "ExclamationMark.h"

ExclamationMark::ExclamationMark(void) : Object3D("Resources/model/exclamationMark/exclamationMark.obj")
{
    // 初期姿勢
    axes_ = Axis3::Initialize();

    transform_.scale = { 0.3f,0.17f,0.3f };
    transform_.rotation = { 0.f,0.f,0.f };
    transform_.position = { 0.f,0.f,0.f };
}

void ExclamationMark::Update(void)
{
    // 姿勢（axes_)の要素が全て0ではない
    const bool isNonZeroAll = axes_.forward.IsNonZero() && axes_.right.IsNonZero() && vec3_newUp_.IsNonZero();

    // 姿勢の調整
    axes_.up = vec3_newUp_; // 新規上ベクトルの設定。
    if (isNonZeroAll)
    {
        axes_.right = Math::Vec3::Cross(axes_.up, axes_.forward).Normalize(); // 新規右ベクトルの設定。
        axes_.forward = Math::Vec3::Cross(axes_.right, axes_.up).Normalize(); // 新規正面ベクトルの設定
    }

    matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_, &matTrans_);

    Object3D::Update();
}

void ExclamationMark::Draw(void)
{
    Object3D::Draw();
}
