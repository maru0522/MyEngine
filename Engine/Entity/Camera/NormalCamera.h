#pragma once
#include "ICamera.h"
/**
 * @file NormalCamera.h
 * @brief 最もベーシックなカメラのクラスが宣言されたファイル
 */


class NormalCamera : public ICamera
{
public:
    //>> 関数
    NormalCamera(const std::string& arg_id);
    virtual ~NormalCamera(void) = default;
    void Update(void) override;

    //>> 変数

};

