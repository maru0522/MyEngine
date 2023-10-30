#include "PostEffectManager.h"
#include "GaussianBlur.h"
#include "RadialBlur.h"
#include "HighLumi.h"
#include "Bloom.h"

PostEffectManager* PostEffectManager::GetInstance(void)
{
    static PostEffectManager ins;
    return &ins;
}

void PostEffectManager::RequestChangePostEffect(PostEffectType arg_type, float arg_v)
{
    // 指定されたタイプに応じたポストエフェクトを生成する
    switch (arg_type)
    {
    case PostEffectType::NONE:
        postEffect_.reset();
        break;
    case PostEffectType::GAUSSIAN_BLUR:
        postEffect_ = std::make_unique<GaussianBlur>();
        break;
    case PostEffectType::HIGH_LUMINANCE:
        postEffect_ = std::make_unique<HighLumi>();
        break;
    case PostEffectType::BLOOM:
        postEffect_ = std::make_unique<Bloom>();
        break;
    case PostEffectType::RADIALBLUR:
        // ブラーが最初からどの程度かかっているか
        postEffect_ = std::make_unique<RadialBlur>(arg_v);
        break;
    default:
        postEffect_ = std::make_unique<PostEffect>();
        break;
    }

    // 新規生成された場合、初期化
    if (postEffect_.get()) { postEffect_->Initialize(); }
}

void PostEffectManager::Update(void)
{
    postEffect_->Update();
}

void PostEffectManager::Draw(Usage arg_usage)
{
    switch (arg_usage)
    {
    case PostEffectManager::Usage::PRE_DRAW:
        postEffect_->PreDrawScene();
        break;
    case PostEffectManager::Usage::POST_DRAW:
        postEffect_->PostDrawScene();
        break;
    case PostEffectManager::Usage::DRAW:
        postEffect_->Draw();
        break;
    default:
        break;
    }
}
