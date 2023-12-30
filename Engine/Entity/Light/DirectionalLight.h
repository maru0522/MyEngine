#pragma once
#include "Vector3.h"
#include "ConstBuffer.h"
/**
 * @file DirectionalLight.h
 * @brief 平行光源についてのクラスが宣言されたファイル
 */

class DirectionalLight
{
private:
    //>> 定義
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
    //>> 変数
    Vector3 direction_{};
    Vector3 color_{};
    bool is_active_{};

public:
    //>> setter・getter
    void SetLightDir(const Vector3& arg_lightDir) { direction_ = arg_lightDir.Normalize(); }
    void SetLightColor(const Vector3& arg_lightColor) { color_ = arg_lightColor; }
    void SetIsActive(bool arg_isActive) { is_active_ = arg_isActive; }

    const Vector3& GetLightDir(void) { return direction_; }
    const Vector3& GetLightColor(void) { return color_; }
    bool GetIsActive(void) { return is_active_; }
};

