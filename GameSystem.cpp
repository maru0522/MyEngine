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
        // -----�����_�[�e�N�X�`���֏�������----- //
        postEffect_->PreDrawScene();
        // --�w�i2DUI�̕`��-- //
        Sprite::PreDraw();
        sceneM_->Draw2dBack();
        // --�w�i2DUI�̕`��-- //

        // --3D�I�u�W�F�N�g�̕`��-- //
        Object3D::PreDraw();
        sceneM_->Draw3d();
        // --3D�I�u�W�F�N�g�̕`��-- //
        postEffect_->PostDrawScene();
        // -----�����_�[�e�N�X�`���֏�������----- //


        // -----�|�X�g�G�t�F�N�g�̕`��----- //
        iDXPtr_->PreDraw();

        postEffect_->Draw();

        // --�O�i2DUI�̕`��-- //
        Sprite::PreDraw();
        sceneM_->Draw2dFore();
        // --�O�i2DUI�̕`��-- //

        imguiController_->Draw();

        iDXPtr_->PostDraw();
        // -----�|�X�g�G�t�F�N�g�̕`��----- //
    }
    else {
        iDXPtr_->PreDraw();
        // --�w�i2DUI�̕`��-- //
        Sprite::PreDraw();
        sceneM_->Draw2dBack();
        // --�w�i2DUI�̕`��-- //

        // --3D�I�u�W�F�N�g�̕`��-- //
        Object3D::PreDraw();
        sceneM_->Draw3d();
        // --3D�I�u�W�F�N�g�̕`��-- //

        // --�O�i2DUI�̕`��-- //
        Sprite::PreDraw();
        sceneM_->Draw2dFore();
        // --�O�i2DUI�̕`��-- //

        imguiController_->Draw();
        iDXPtr_->PostDraw();
    }
}

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
