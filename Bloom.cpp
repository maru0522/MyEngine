#include "Bloom.h"

Bloom::Bloom(void)
    : PostEffect()
{
}

void Bloom::Initialize(void)
{
    PostEffect::Initialize();
    PostEffect::SetPSOName("BLOOM");
}

void Bloom::PreDrawScene(void)
{
    PostEffect::PreDrawScene();
}

void Bloom::PostDrawScene(void)
{
    PostEffect::PostDrawScene();
}

void Bloom::Draw(void)
{
    PostEffect::Draw();
}
