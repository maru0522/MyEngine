#pragma once
#include "ICamera.h"
class BehindCamera : public ICamera
{
public:
    //>> 関数
    BehindCamera(const std::string& arg_id);
    virtual ~BehindCamera(void) = default;
    void Update(void) override;

    //>> 変数
    Vector3 pos_player_;
    Axis3 axes_player_;
};

