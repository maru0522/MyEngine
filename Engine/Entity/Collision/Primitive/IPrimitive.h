#pragma once

enum class Shape
{
    UNKNOWN = -1, // 未定義
    SPHERE,       // 球
    PLANE,        // 平面
    POINT,        // 点
    AABB,         // XYZ軸に平行な直方体
    OBB,          // 分離軸に平行な直方体
    RAY,          // 半直線
};

class IPrimitive
{
public:
    //>> 関数
    IPrimitive(Shape arg_shape) : shape_(arg_shape) {}
    virtual ~IPrimitive(void) = default;

protected:
    Shape shape_;

public:
    // getter
    Shape GetShape(void) { return shape_; }
};

