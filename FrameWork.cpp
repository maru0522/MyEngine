#include "FrameWork.h"
#include "Object3D.h"
#include "Sprite.h"
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
    // GraphicsPipeline������
    gplPtr_->Initialize();

    // Sprite������
    Sprite::StaticInitialize(texM_.get());
    // ModelManager - 3D���f���ǂݍ���
    modelM_->LoadOBJ("Resources/cube/cube.obj");
    // TextureManager - �e�N�X�`���ǂݍ���
    texM_->LoadFolder("Resources");
    // Object3D������
    Object3D::StaticInitialize(modelM_.get(), texM_.get());

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
