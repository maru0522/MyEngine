#include "SimplifyImGui.h"
#include "GameSystem.h"
#include "Object3D.h"
#include "Sprite.h"

void GameSystem::Initialize()
{
    FrameWork::Initialize();
}

void GameSystem::Update(void)
{
    FrameWork::Update();
}

void GameSystem::Draw(void)
{
    if (postEffect_.get()) {
        // -----ƒŒƒ“ƒ_[ƒeƒNƒXƒ`ƒƒ‚Ö‘‚«ž‚Ý----- //
        postEffect_->PreDrawScene();
        // --”wŒi2DUI‚Ì•`‰æ-- //
        Sprite::PreDraw();
        sceneM_->Draw2dBack();
        // --”wŒi2DUI‚Ì•`‰æ-- //

        // --3DƒIƒuƒWƒFƒNƒg‚Ì•`‰æ-- //
        Object3D::PreDraw();
        sceneM_->Draw3d();
        // --3DƒIƒuƒWƒFƒNƒg‚Ì•`‰æ-- //
        postEffect_->PostDrawScene();
        // -----ƒŒƒ“ƒ_[ƒeƒNƒXƒ`ƒƒ‚Ö‘‚«ž‚Ý----- //


        // -----ƒ|ƒXƒgƒGƒtƒFƒNƒg‚Ì•`‰æ----- //
        iDXPtr_->PreDraw();

        postEffect_->Draw();

        // --‘OŒi2DUI‚Ì•`‰æ-- //
        Sprite::PreDraw();
        sceneM_->Draw2dFore();
        // --‘OŒi2DUI‚Ì•`‰æ-- //

        imguiController_->Draw();

        iDXPtr_->PostDraw();
        // -----ƒ|ƒXƒgƒGƒtƒFƒNƒg‚Ì•`‰æ----- //
    }
    else {
        iDXPtr_->PreDraw();
        // --”wŒi2DUI‚Ì•`‰æ-- //
        Sprite::PreDraw();
        sceneM_->Draw2dBack();
        // --”wŒi2DUI‚Ì•`‰æ-- //

        // --3DƒIƒuƒWƒFƒNƒg‚Ì•`‰æ-- //
        Object3D::PreDraw();
        sceneM_->Draw3d();
        // --3DƒIƒuƒWƒFƒNƒg‚Ì•`‰æ-- //

        // --‘OŒi2DUI‚Ì•`‰æ-- //
        Sprite::PreDraw();
        sceneM_->Draw2dFore();
        // --‘OŒi2DUI‚Ì•`‰æ-- //

        imguiController_->Draw();
        iDXPtr_->PostDraw();
    }
}

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
