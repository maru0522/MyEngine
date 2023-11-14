#include "OBB.h"

Primitive::OBB::OBB(const Vector3& arg_center, const Vector3* const arg_orien[3], const Vector3& arg_radius)
    : IPrimitive(Shape::OBB),center(arg_center), radius(arg_radius) 
{
    orientations[0] = *arg_orien[0];
    orientations[1] = *arg_orien[1];
    orientations[2] = *arg_orien[2];
}
