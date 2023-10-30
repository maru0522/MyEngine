#include "CollisionManager.h"
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
    CollisionManager::GetInstance()->Update();
    if (postEffectMPtr_->GetPostEffectPtr())
    {
        postEffectMPtr_->GetPostEffectPtr()->Update();
    }
}

void GameSystem::Draw(void)
{
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

        postEffectMPtr_->Draw(PostEffectManager::Usage::DRAW);

        // --前景2DUIの描画-- //
        Sprite::PreDraw();
        SceneManager::GetInstance()->Draw2dFore();
        // --前景2DUIの描画-- //

        imguiController_->Draw();

        iDXPtr_->PostDraw();
        // -----ポストエフェクトの描画----- //
    }
    else {
        iDXPtr_->PreDraw();
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
        // --前景2DUIの描画-- //

        imguiController_->Draw();
        iDXPtr_->PostDraw();
    }
}

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
