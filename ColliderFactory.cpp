#include "ColliderFactory.h"
#include "CollisionPrimitive.h"

std::unique_ptr<ICollider> ColliderFactory::CreateCollider(Shape shape)
{
    if (shape == Shape::SPHERE) { return std::make_unique<CollisionPrimitive::SphereCollider>(); }
    if (shape == Shape::PLANE) { return std::make_unique<CollisionPrimitive::PlaneCollider>(); }

    // default: ‹…
    return std::make_unique<CollisionPrimitive::SphereCollider>();
}
