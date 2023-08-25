#pragma once
#include <string>
#include "Object3D.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

// ‰¼’u‚«‚ÌƒNƒ‰ƒX
class IventArea_Tantative : public Object3D
{
public:
    // ’è”
    const Vector3 kRadius_{ 5,5,5 };

    // ŠÖ”
    IventArea_Tantative(CollisionManager* colMPtr,const std::string& id);
    ~IventArea_Tantative(void) = default;

    void Update(void) override;
    void Draw(void) override;

    // •Ï”
    Transform transform_;
    Axis3 axes_;
    CollisionPrimitive::AABBCollider aabb_;

    void OnCollision(void); // callback
};

