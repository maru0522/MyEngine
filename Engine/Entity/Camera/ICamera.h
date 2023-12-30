#pragma once
#include "Transform.h"
#include <string>
#include "SceneNameList.h"
#include "Screen.h"
/**
 * @file ICamera.h
 * @brief 様々なカメラクラスの基底となるカメラクラスが宣言されたファイル
 */

// 前方宣言
class CameraManager; // 前方宣言なのでファイル内に隠蔽

class ICamera
{
private:
    //>> 定義
    friend CameraManager;

    const float kDef_farZ_ = 15000.f;
    const float kDef_nearZ_ = 0.1f;

public:
    //>> 関数
    ICamera(void) : ICamera("ICamera_") {};
    ICamera(const std::string& arg_id);
    virtual ~ICamera(void) = default;
    virtual void Update(void);

private:
    void UpdateOrthoGraphic(void); //** cameraManager用

protected:
    //>> 変数
    std::string id_;
    Vector3 vec_target_;
    SceneName birthScene_;

    Axis3 axes_; // 姿勢
    Transform transform_; // 座標など
    TransformMatrix transformMatrix_; // 行列化したtransform情報

    float farZ_;
    float nearZ_;
    Matrix4 matView_;
    Matrix4 matProj_Perspective_;
    Matrix4 matProj_OrthoGraphic_;
    bool is_affinUseAxes_;

    Screen screen_;

public:
    // setter
    void SetId(const std::string& arg_id) { id_ = arg_id; }

    void SetFarZ(float arg_farZ) { farZ_ = arg_farZ; }
    void SetNearZ(float arg_nearZ) { nearZ_ = arg_nearZ; }

    void SetAxis3(const Axis3& arg_axes) { axes_ = arg_axes; }
    void SetPosition(const Vector3& arg_pos) { transform_.position = arg_pos; }
    void SetTransform(const Transform& arg_transform) { transform_ = arg_transform; }
    void SetTransformMatrix(const TransformMatrix& arg_transMat) { transformMatrix_ = arg_transMat; }
    void SetTargetPos(const Vector3& arg_targetPos) { vec_target_ = arg_targetPos; }

    void SetIsAffinUseAxes(bool arg_isAffinUseAxes) { is_affinUseAxes_ = arg_isAffinUseAxes; }

    // getter
    const std::string& GetId(void) { return id_; }

    const Axis3& GetAxis3(void) { return axes_; }
    const Transform& GetTransform(void) { return transform_; }
    const TransformMatrix& GetTransformMatrix(void) { return transformMatrix_; }

    bool GetIsAffinUseAxes(void) { return is_affinUseAxes_; }

    const Matrix4& GetMatView(void) { return matView_; }
    const Matrix4& GetMatProjPerspective(void) { return matProj_Perspective_; }
    const Matrix4& GetMatProjOrthoGraphic(void) { return matProj_OrthoGraphic_; }

    float GetFarZ(void) { return farZ_; }
    float GetNearZ(void) { return nearZ_; }

    const Screen& GetScreen(void) { return screen_; }
};

