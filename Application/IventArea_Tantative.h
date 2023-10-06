#pragma once
#include <string>
#include "Object3D.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

// 仮置きのクラス
class IventArea_Tantative : public Object3D
{
public:
    // 定数
    const Vector3 kRadius_{ 5,5,5 };

    // 関数
    IventArea_Tantative(CollisionManager* colMPtr,const std::string& id);
    ~IventArea_Tantative(void);

    void Update(void) override;
    void Draw(void) override;

    // 変数
    Transform transform_;
    Axis3 axes_;
    CollisionManager* colMPtr_;
    CollisionPrimitive::AABBCollider aabb_;

    void OnCollision(void); // callback
};

