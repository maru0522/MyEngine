#pragma once
#include "Vector2.h"
#include "Vector3.h"

namespace CollisionPrimitive
{
    struct Sphere
    {
        Vector3 center{ 0, 0, 0 };
        float radius{ 0.f };
        bool isHit{};

        constexpr Sphere(void) noexcept = default;
    };

    struct Plane
    {
        Vector3 normal{ 0, 1, 0 };
        float distance{ 0.f };

        constexpr Plane(void) noexcept = default;
    };
}