#pragma once
#include "Vector3.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

class ColliderCamera : public Camera
{
public:
    const float kColRadius_{ 3.f };

    //>> 関数
    ColliderCamera(void);
    ~ColliderCamera(void);
    void Update(void) override;

private:
    //>> 変数
    bool is_oldUpdateMethod_{};

public:
    //>> setter

    // プレイヤーの座標からプレイヤー方向へのベクトルを正面ベクトルとして算出する
    void CalcAxis3(const Vector3& playerPos, const Vector3& pUpVec);

    void SetViewMat(const Matrix4& viewMat) { matView_ = viewMat; }

    void SetIsOldUpdateMethod(bool is_oldUpdateMethod) { is_oldUpdateMethod_ = is_oldUpdateMethod; }
};

