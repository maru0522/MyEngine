#include "PointCollider.h"

CollisionPrimitive::PointCollider::PointCollider(const std::string& arg_id)
    : Point(), ICollider()
{
    id_ = arg_id;
}