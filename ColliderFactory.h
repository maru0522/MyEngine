#pragma once
#include <memory>
#include "ICollider.h"

class ColliderFactory
{
public:
    // íËã`
    enum class Shape
    {
        SPHERE,
        PLANE,
    };

    // ä÷êî
    ColliderFactory(void) noexcept = default;
    ~ColliderFactory(void) noexcept = default;

    std::unique_ptr<ICollider> CreateCollider(Shape shape);
};

