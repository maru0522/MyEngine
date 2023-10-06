#pragma once
#include "Vector3.h"
#include "ConstBuffer.h"

class DirectionalLight
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    struct CBDirLight_t
    {
        Vector3 lightV;
        float pad1;
        Vector3 lightColor;
        uint32_t isActive;
    };

private:
    // 変数
    Vector3 lightDir_{ 1,0,0 };
    Vector3 lightColor_{ 1,1,1 };

    bool isActive_{};

public:
    // setter・getter
    inline void SetLightDir(const Vector3& lightDir) { lightDir_ = lightDir.Normalize(); }
    inline void SetLightColor(const Vector3& lightColor) { lightColor_ = lightColor; }
    inline void SetIsActive(bool isActive) { isActive_ = isActive; }

    inline const Vector3& GetLightDir(void) { return lightDir_; }
    inline const Vector3& GetLightColor(void) { return lightColor_; }
    inline bool GetIsActive(void) { return isActive_; }
};

