#pragma once
#include <unordered_map>

class CollisionObserver final
{
public:
    //>> 関数
    CollisionObserver(void) = default;
    ~CollisionObserver(void) = default;

    //>> 変数
    std::unordered_map<char*, bool> map_;
};

