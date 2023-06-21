#include "GameSystem.h"
#include "Sprite.h"
#include "Object3D.h"

void GameSystem::Initialize()
{
    FrameWork::Initialize();
    postEffect_ = std::make_unique<PostEffect>();
}

void GameSystem::Update(void)
{
    FrameWork::Update();
}

void GameSystem::Draw(void)
{
    iDXPtr_->PreDraw();

    // ---------2DUIの描画--------- //
    Sprite::PreDraw();
    //sceneM_->Draw2d();
    // ---------2DUIの描画--------- //


    // -----3Dオブジェクトの描画----- //
    Object3D::PreDraw();
    //sceneM_->Draw3d();

    // -----3Dオブジェクトの描画----- //

    postEffect_->Draw();
    imguiController_->Draw();

    iDXPtr_->PostDraw();
}

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
