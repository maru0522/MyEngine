#include "HighLumi.h"

HighLumi::HighLumi(void)
    : PostEffect()
{
}

void HighLumi::Initialize(void)
{
    PostEffect::Initialize();
    PostEffect::SetPSOName("HIGHLUMI");
}

void HighLumi::PreDrawScene(void)
{
    PostEffect::PreDrawScene();
}

void HighLumi::PostDrawScene(void)
{
    PostEffect::PostDrawScene();
}

void HighLumi::Draw(void)
{
    PostEffect::Draw();
}
