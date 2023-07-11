#pragma once
#include <memory>
#include "ICollider.h"

class ColliderFactory
{
public:
    // ��`
    enum class Shape
    {
        SPHERE,
        PLANE,
    };

    // �֐�
    ColliderFactory(void) noexcept = default;
    ~ColliderFactory(void) noexcept = default;

    std::unique_ptr<ICollider> CreateCollider(Shape shape);
};

