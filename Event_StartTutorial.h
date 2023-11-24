#pragma once
#include "Sprite.h"

class Event_StartTutorial
{
public:
    //>> 関数
    Event_StartTutorial(void);
    ~Event_StartTutorial(void) = default;

    void Execute(void);
    void Draw(void);
private:
    //>> 変数
    std::array<std::unique_ptr<Sprite>,2> cinemas_;
};

