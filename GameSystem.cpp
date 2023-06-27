#include "GameSystem.h"
#include "Sprite.h"
#include "Object3D.h"

void GameSystem::Initialize()
{
    FrameWork::Initialize();
    postEffect_ = std::make_unique<GaussianBlur>();
    postEffect_->Initialize();
}

void GameSystem::Update(void)
{
    FrameWork::Update();
}

void GameSystem::Draw(void)
{
    // -----レンダーテクスチャへ書き込み----- //
    postEffect_->PreDrawScene();
    // --背景2DUIの描画-- //
    Sprite::PreDraw();
    sceneM_->Draw2dBack();
    // --背景2DUIの描画-- //

    // --3Dオブジェクトの描画-- //
    Object3D::PreDraw();
    sceneM_->Draw3d();
    // --3Dオブジェクトの描画-- //
    postEffect_->PostDrawScene();
    // -----レンダーテクスチャへ書き込み----- //


    // -----ポストエフェクトの描画----- //
    iDXPtr_->PreDraw();

    postEffect_->Draw();

    // --前景2DUIの描画-- //
    Sprite::PreDraw();
    sceneM_->Draw2dFore();
    // --前景2DUIの描画-- //

    imguiController_->Draw();

    iDXPtr_->PostDraw();
    // -----ポストエフェクトの描画----- //
}

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
