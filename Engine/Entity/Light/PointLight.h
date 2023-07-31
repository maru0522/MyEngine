#pragma once
#include "Vector3.h"

class PointLight
{
public:
    // íËã`
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
    // ïœêî
    Vector3 lightPos_;
    Vector3 lightColor_;
    Vector3 lightAtten_;
    bool isActive_{};

public:
    // setterÅEgetter
    inline void SetIsActive(bool isActive) { isActive_ = isActive; }
    inline void SetLightPos(const Vector3& lightpos) { lightPos_ = lightpos; }
    inline void SetLightColor(const Vector3& lightcolor) { lightColor_ = lightcolor; }
    inline void SetLightAtten(const Vector3& lightatten) { lightAtten_ = lightatten; }

    inline bool GetIsActive(void) { return isActive_; }
    inline const Vector3& GetLightPos(void) { return lightPos_; }
    inline const Vector3& GetLightColor(void) { return lightColor_; }
    inline const Vector3& GetLightAtten(void) { return lightAtten_; }
};

