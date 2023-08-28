#pragma once
#include <functional>

class IIvent
{
public:
    IIvent(void) = default;
    virtual ~IIvent(void) = default;

    std::function<void(void)> funcion_event_{};
};

