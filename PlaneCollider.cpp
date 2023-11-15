#include "PlaneCollider.h"

CollisionPrimitive::PlaneCollider::PlaneCollider(const std::string& arg_id)
    : Plane(),ICollider()
{
    id_ = arg_id;
}
