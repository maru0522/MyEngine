#include "AABBCollider.h"

CollisionPrimitive::AABBCollider::AABBCollider(const std::string& arg_id)
    : AABB(),ICollider()
{
    id_ = arg_id;
}