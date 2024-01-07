#pragma once
#include "Object3D.h"
/**
 * @file ExclamationMark.h
 * @brief ビックリマーク[!]クラス
 */
class ExclamationMark :
    public Object3D
{
public:
    //>> 関数
    ExclamationMark(void);
    ~ExclamationMark(void) = default;

    void Update(void) override;
    void Draw(void) override;

private:
    //>> 変数
    Axis3 axes_;
    Vector3 vec3_newUp_; // 新規上ベクトル
    Transform transform_;

public:
    //>> setter
    void SetNewUp(const Vector3& arg_newUp) { vec3_newUp_ = arg_newUp; }
    void SetTransform(const Transform& arg_transform) { transform_ = arg_transform; }
    void SetAxes(const Axis3& arg_axes) { axes_ = arg_axes; }

    //>> getter
    const Transform& GetTransform(void) { return transform_; }
    const Axis3& GetAxes(void) { return axes_; }
};

