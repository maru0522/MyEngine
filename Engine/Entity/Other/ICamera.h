#pragma once
#include "Transform.h"
#include <string>

// 前方宣言
class CameraManager; // 前方宣言なのでファイル内に隠蔽

class ICamera
{
private:
    //>> 定義
    friend CameraManager;

    const float kDef_farZ_ = 10000.f;
    const float kDef_nearZ_ = 0.1f;

public:
    //>> 関数
    ICamera(void) : ICamera("ICamera") {};
    ICamera(const std::string& arg_id);
    virtual ~ICamera(void) = default;
    virtual void Update(void);

private:
    void UpdateOrthoGraphic(void); //** cameraManager用

protected:
    //>> 変数
    std::string id_;
    Vector3 vec_target_;

    Axis3 axes_; // 姿勢
    Transform transform_; // 座標など
    TransformMatrix transformMatrix; // 行列化したtransform情報

    float farZ_;
    float nearZ_;
    Matrix4 matView_;
    Matrix4 matProj_Perspective_;
    Matrix4 matProj_OrthoGraphic_;

public:
    // setter
    void SetId(const std::string& arg_id) { id_ = arg_id; }

    void SetFarZ(float arg_farZ) { farZ_ = arg_farZ; }
    void SetNearZ(float arg_nearZ) { nearZ_ = arg_nearZ; }

    void SetAxis3(const Axis3& arg_axes) { axes_ = arg_axes; }
    void SetTransform(const Transform& arg_transform) { transform_ = arg_transform; }

    // getter
    const std::string& GetId(void) { return id_; }

    const Axis3& GetAxis3(void) { return axes_; }
    const Transform& GetTransform(void) { return transform_; }
    const TransformMatrix& GetCoordinate(void) { return transformMatrix; }

    const Matrix4& GetMatView(void) { return matView_; }
    const Matrix4& GetMatProjPerspective(void) { return matProj_Perspective_; }
    const Matrix4& GetMatProjOrthoGraphic(void) { return matProj_OrthoGraphic_; }

    float GetFarZ(void) { return farZ_; }
    float GetNearZ(void) { return nearZ_; }
};
