#include "CollisionManager.h"
#include "SimplifyImGui.h"
#include "GameSystem.h"
#include "Object3D.h"
#include "Sprite.h"
#include "BackGroundTexture.h"

void GameSystem::Initialize()
{
    FrameWork::Initialize();
    BackGroundTexture::GetInstance()->Initialize();
}

void GameSystem::Update(void)
{
    FrameWork::Update();
    CollisionManager::GetInstance()->Update();
    BackGroundTexture::GetInstance()->Update();
    if (postEffectMPtr_->GetPostEffectPtr())
    {
        postEffectMPtr_->GetPostEffectPtr()->Update();
    }
}

void GameSystem::Draw(void)
{
    //BackGroundTexture::GetInstance()->BeginWrite();
    //Object3D::PreDraw();
    //BackGroundTexture::GetInstance()->ExecuteWrite();
    //BackGroundTexture::GetInstance()->EndWrite();

    if (postEffectMPtr_->GetPostEffectPtr()) {
        // -----レンダーテクスチャへ書き込み----- //
        postEffectMPtr_->Draw(PostEffectManager::Usage::PRE_DRAW);
        // --背景2DUIの描画-- //
        Sprite::PreDraw();
        SceneManager::GetInstance()->Draw2dBack();
        // --背景2DUIの描画-- //

        // --3Dオブジェクトの描画-- //
        Object3D::PreDraw();
        SceneManager::GetInstance()->Draw3d();
        // --3Dオブジェクトの描画-- //
        postEffectMPtr_->Draw(PostEffectManager::Usage::POST_DRAW);
        // -----レンダーテクスチャへ書き込み----- //


        // -----ポストエフェクトの描画----- //
        iDXPtr_->PreDraw();
        //BackGroundTexture::GetInstance()->Draw();

        postEffectMPtr_->Draw(PostEffectManager::Usage::DRAW);

        // --前景2DUIの描画-- //
        Sprite::PreDraw();
        SceneManager::GetInstance()->Draw2dFore();
        figureUIPtr_->Draw();
        // --前景2DUIの描画-- //

        imguiController_->Draw();

        iDXPtr_->PostDraw();
        // -----ポストエフェクトの描画----- //
    }
    else {
        iDXPtr_->PreDraw();
        //BackGroundTexture::GetInstance()->Draw();                                                     

        // --背景2DUIの描画-- //
        Sprite::PreDraw();
        SceneManager::GetInstance()->Draw2dBack();
        // --背景2DUIの描画-- //

        // --3Dオブジェクトの描画-- //
        Object3D::PreDraw();
        SceneManager::GetInstance()->Draw3d();
        // --3Dオブジェクトの描画-- //

        // --前景2DUIの描画-- //
        Sprite::PreDraw();
        SceneManager::GetInstance()->Draw2dFore();
        figureUIPtr_->Draw();
        // --前景2DUIの描画-- //

        imguiController_->Draw();
        iDXPtr_->PostDraw();
    }
}

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
