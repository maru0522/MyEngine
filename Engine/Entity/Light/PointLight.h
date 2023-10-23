#pragma once
#include "Vector3.h"

class PointLight
{
public:
    //>> 定義
    struct CBPointLight_t
    {
        Vector3 lightPos;
        float pad1;
        Vector3 lightColor;
        float pad2;
        Vector3 lightAtten;
        uint32_t isActive;
    };

private:
    //>> 変数
    Vector3 position_{};
    Vector3 color_{};
    Vector3 attan_{};
    bool is_active_{};

public:
    //>> setter・getter
    void SetIsActive(bool arg_isActive) { is_active_ = arg_isActive; }
    void SetLightPos(const Vector3& arg_lightPos) { position_ = arg_lightPos; }
    void SetLightColor(const Vector3& arg_lightColor) { color_ = arg_lightColor; }
    void SetLightAtten(const Vector3& arg_lightAtten) { attan_ = arg_lightAtten; }

    bool GetIsActive(void) { return is_active_; }
    const Vector3& GetLightPos(void) { return position_; }
    const Vector3& GetLightColor(void) { return color_; }
    const Vector3& GetLightAtten(void) { return attan_; }
};

