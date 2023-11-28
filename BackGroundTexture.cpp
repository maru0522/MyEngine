#include "BackGroundTexture.h"

BackGroundTexture* BackGroundTexture::GetInstance(void)
{
    static BackGroundTexture ins;
    return &ins;
}

void BackGroundTexture::Initialize(void)
{
    PostEffect::Initialize();
}

void BackGroundTexture::Update(void)
{
    if (func_draw_) { is_setFunc_ = true; }
}

void BackGroundTexture::BeginWrite(void)
{
    if (is_setFunc_ == false) { return; }
    PostEffect::PreDrawScene();
}

void BackGroundTexture::ExecuteWrite(void)
{
    if (is_setFunc_ == false) { return; }

    func_draw_();
}

void BackGroundTexture::EndWrite(void)
{
    if (is_setFunc_ == false) { return; }
    PostEffect::PostDrawScene();
}

void BackGroundTexture::Draw(void)
{
    if (is_setFunc_ == false) { return; }
    PostEffect::Draw();
}
