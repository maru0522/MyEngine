#include "RadialBlur.h"

RadialBlur::RadialBlur(void)
    : PostEffect()
{
}

void RadialBlur::Initialize(void)
{
    PostEffect::Initialize();
    PostEffect::SetPSOName("RADIAL");
}

void RadialBlur::PreDrawScene(void)
{
    PostEffect::PreDrawScene();
}

void RadialBlur::PostDrawScene(void)
{
    PostEffect::PostDrawScene();
}

void RadialBlur::Draw(void)
{
    PostEffect::Draw();
}
