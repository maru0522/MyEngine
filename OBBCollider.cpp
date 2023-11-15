#include "OBBCollider.h"

CollisionPrimitive::OBBCollider::OBBCollider(const std::string& arg_id)
    : OBB(),ICollider()
{
    id_ = arg_id;
}