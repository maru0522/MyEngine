#include "DemoScene.h"

void DemoScene::Initialize(void)
{
}

void DemoScene::Update(void)
{
    sprite_->Update();
    obj3d_->Update();
}

void DemoScene::Draw3d(void)
{
    obj3d_->Draw();
    // cubeの画像にスプライトで表示しているものを起用
    //obj3d_->Draw("Resources/namida.png");
}

void DemoScene::Draw2d(void)
{
    sprite_->Draw();
}

void DemoScene::Finalize(void)
{
}
