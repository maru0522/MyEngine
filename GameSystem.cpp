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

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
