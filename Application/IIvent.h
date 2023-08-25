#pragma once
#include <functional>

class IIvent
{
public:
    IIvent(void) = default;
    virtual ~IIvent(void) = default;

    //template<typename T,typename U>
    //std::function<T(U)> onCollision_{};
};

