#pragma once
#include "ICamera.h"
class NormalCamera : public ICamera
{
public:
    //>> 関数
    NormalCamera(const std::string& arg_id);
    virtual ~NormalCamera(void) = default;
    void Update(void);

    //>> 変数

};

