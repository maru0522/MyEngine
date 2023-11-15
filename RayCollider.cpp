#include "RayCollider.h"

CollisionPrimitive::RayCollider::RayCollider(const std::string& arg_id)
    : Ray(), ICollider()
{
    id_ = arg_id;
}