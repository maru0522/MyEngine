#include "DemoScene.h"
#include "Input.h"

void DemoScene::Initialize(void)
{
    // �J�����̍��W�ݒ�
    cameraPtr->SetPosition({ 0,0,-20 });
    // �J�������}�l�[�W���[�ɃZ�b�g
    CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());
}

void DemoScene::Update(void)
{
    sprite_->Update();
    obj3d_->Update();

    if (KEYS::IsTrigger(DIK_P)) sound_->Play(1);
    if (KEYS::IsTrigger(DIK_O)) music_->Play();
    if (KEYS::IsTrigger(DIK_I)) music_->Stop();
    if (KEYS::IsTrigger(DIK_U)) music_->SetPitch(2.f);
}

void DemoScene::Draw3d(void)
{
    obj3d_->Draw();
    // cube�̉摜�ɃX�v���C�g�ŕ\�����Ă�����̂��N�p
    //obj3d_->Draw("Resources/namida.png");
}

void DemoScene::Draw2d(void)
{
    sprite_->Draw();
}

void DemoScene::Finalize(void)
{
}
