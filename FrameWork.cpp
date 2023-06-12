#include "FrameWork.h"
#include "Object3D.h"
#include "Sprite.h"
#include "Sound.h"
#include "Input.h"

void FrameWork::Run(void)
{
    Initialize();

    while (true)
    {
        Update();

        if (EndRequest()) {
            break;
        }

        Draw();
    }

    Finalize();
}

void FrameWork::Initialize()
{
    // InitDirectX������
    iDXPtr_->Initialize(wnd_.get());
    // ���͌n�N���X�S�ď�����
    Input::InitializeAll(wnd_.get());
    // TextureManager������
    texM_->Initialize();
    // TextureManager - �e�N�X�`���ǂݍ���
    texM_->LoadFolder("Resources");
    // ModelManager - 3D���f���ǂݍ���
    modelM_->LoadOBJ("Resources/model/cube/cube.obj",false);
    modelM_->LoadOBJ("Resources/model/ICOSphere/ICOSphere.obj",false);
    // AudioManager - ���y&SE�ǂݍ���
    audioM_->LoadFolder("Resources/sound");
    // GraphicsPipeline������
    gplPtr_->Initialize();

    // Sprite������
    Sprite::StaticInitialize(texM_.get());
    // Object3D������
    Object3D::StaticInitialize(modelM_.get(), texM_.get());
    // Sound������
    Sound::StaticInitalize(audioM_.get());

    // SceneManager������
    sceneM_->Initialize(SceneFactory::Usage::DEMO);
    // ImguiController������
    imguiController_->Initialize(wnd_.get());
}

void FrameWork::Update(void)
{
    // ���͌n�N���X�S�čX�V
    Input::UpdateAll();

    // ImguiController�X�V�J�n
    imguiController_->Begin();

    // SceneManager�X�V
    sceneM_->Update();

    // ImguiController�X�V�I��
    imguiController_->End();

    // CameraManager�X�V
    CameraManager::GetInstance()->Update();

    // �J�����̓������e�s���Object3D�p�̒萔�ɑ���
    Object3D::UpdateCBMatViewPerse();
}

void FrameWork::Finalize(void)
{
    // �E�B���h�E�N���X��o�^����
    wnd_->Delete();
}

bool FrameWork::EndRequest(void)
{
    return !wnd_->GetKeepWindow();
}
