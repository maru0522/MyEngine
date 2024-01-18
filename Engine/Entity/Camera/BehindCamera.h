#pragma once
#include "ICamera.h"
/**
 * @file BehindCamera.h
 * @brief プレイヤーの頭上から追従するクラスが宣言されたファイル
 */

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

    float distance_fromPlayer_;

    //>> setter
    void SetDistance(float arg_distFromPlayer) { distance_fromPlayer_ = arg_distFromPlayer; }

    //>> getter
    float GetDistance(void) { return distance_fromPlayer_; }
};

