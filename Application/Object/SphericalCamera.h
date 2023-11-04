#pragma once
#include "Vector3.h"
#include "Player.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

class SphericalCamera : public Camera
{
public:
    //>> 関数
    SphericalCamera(void);
    ~SphericalCamera(void);
    void Update(void) override;

    float radius_; // 中心点からの距離
    float theta_; // 仰角
    float phi_; // 方位角
    Vector3 pos_target; // 目線の先

    Vector3 vec_playerUp_;

private:
    //>> 変数

public:
    //>> setter

    // プレイヤーの座標からプレイヤー方向へのベクトルを正面ベクトルとして算出する
    void CalcAxis3(const Vector3& playerPos, const Vector3& pUpVec);

    void SetViewMat(const Matrix4& viewMat) { matView_ = viewMat; }

    //void SetIsOldUpdateMethod(bool is_oldUpdateMethod) { is_oldUpdateMethod_ = is_oldUpdateMethod; }

    void Debug_need(const Vector3& arg_spherical, const Vector3& pos,const Vector3& pos_eye);
};

