#pragma once
#include <memory>
#include "PostEffect.h"

enum class PostEffectType
{
    NONE, 
    GAUSSIAN_BLUR,
    HIGH_LUMINANCE,
    BLOOM,
    RADIALBLUR
};

class PostEffectManager final
{
private:
    //>> singleton
    PostEffectManager(void) = default;
    ~PostEffectManager(void) = default;
    PostEffectManager(const PostEffectManager&) = delete;
    PostEffectManager& operator=(const PostEffectManager&) = delete;

public:
    //>> 定義
    enum class Usage
    {
        PRE_DRAW,
        POST_DRAW,
        DRAW
    };

    //>> 関数
    static PostEffectManager* GetInstance(void);

    void RequestChangePostEffect(PostEffectType arg_type, float arg_v = 0.f);
    void Update(void);
    void Draw(Usage arg_usage);

private:
    //>> 変数
    std::unique_ptr<PostEffect> postEffect_;

public:
    //>> getter
    PostEffect* GetPostEffectPtr(void) { return postEffect_.get(); }
};

