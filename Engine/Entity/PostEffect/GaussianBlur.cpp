#include "GaussianBlur.h"

GaussianBlur::GaussianBlur(void) 
    : PostEffect()
{
}

void GaussianBlur::Initialize(void)
{
    PostEffect::Initialize();
    PostEffect::SetPSOName("GAUSSIAN");
}

void GaussianBlur::PreDrawScene(void)
{
    PostEffect::PreDrawScene();
}

void GaussianBlur::PostDrawScene(void)
{
    PostEffect::PostDrawScene();
}

void GaussianBlur::Draw(void)
{
    PostEffect::Draw();
}
