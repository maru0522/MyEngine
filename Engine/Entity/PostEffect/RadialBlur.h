#pragma once
#include "PostEffect.h"
class RadialBlur :
    public PostEffect
{
private:
    //>> 定義
    struct CBData_radialBlur_t
    {
        float blur;
    };

public:
    //>> 関数
    RadialBlur(float arg_blur);
    ~RadialBlur(void) override = default;

    void Initialize(void) override;
    void Update(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;

private:
    //>> 変数
    ConstBuffer<CBData_radialBlur_t> cb_radialBlur_;
    float blur_;

public:
    //>> setter
    void SetBlurValue(float arg_blur) { blur_ = arg_blur; }

    //>> getter
    float GetBlurValue(void) { return blur_; }
};

