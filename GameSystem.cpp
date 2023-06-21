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

    // ---------2DUI�̕`��--------- //
    Sprite::PreDraw();
    //sceneM_->Draw2d();
    // ---------2DUI�̕`��--------- //


    // -----3D�I�u�W�F�N�g�̕`��----- //
    Object3D::PreDraw();
    //sceneM_->Draw3d();

    // -----3D�I�u�W�F�N�g�̕`��----- //

    postEffect_->Draw();
    imguiController_->Draw();

    iDXPtr_->PostDraw();
}

void GameSystem::Finalize(void)
{
    FrameWork::Finalize();
}
