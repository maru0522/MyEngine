#pragma once
#include "ICamera.h"

class DebugCamera final : public ICamera
{
public:
    //>> 関数
    DebugCamera(const std::string& arg_id);
    virtual ~DebugCamera(void) = default;
    virtual void Update(void);

private:
    //>> 変数
    //ICamera* refrenceCameraPtr_;
};