#include "OBBCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::OBBCollider::OBBCollider(const std::string& arg_id)
    : OBB(),ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{
    if (arg_Shpere)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}