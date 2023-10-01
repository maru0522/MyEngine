#include "SimplifyImGui.h"
#include "GaussianBlur.h"
#include "RadialBlur.h"
#include "FrameWork.h"
#include "HighLumi.h"
#include "Object3D.h"
#include "Sprite.h"
#include "Bloom.h"
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
    modelM_->LoadOBJ("Resources/model/cube/cube.obj", false);
    modelM_->LoadOBJ("Resources/model/ICOSphere/ICOSphere.obj", false);
    modelM_->LoadOBJ("Resources/model/lolP/lolP.obj", false);
    modelM_->LoadOBJ("Resources/model/sphere/sphere.obj", true);
    // AudioManager - ���y&SE�ǂݍ���
    audioM_->LoadFolder("Resources/sound");
    // GraphicsPipeline������
    psoMPtr_->Create();

    // Sprite������
    Sprite::StaticInitialize(texM_.get());
    // Object3D������
    Object3D::StaticInitialize(modelM_.get(), texM_.get());
    // Sound������
    Sound::StaticInitalize(audioM_.get());

    // SceneManager������
    SceneManager::GetInstance()->Initialize(SceneFactory::Usage::TITLE);
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
    SceneManager::GetInstance()->Update();
    DebugGui();

    // ImguiController�X�V�I��
    imguiController_->End();

    // CameraManager�X�V
    camMPtr_->Update();

    // �J�����̓������e�s���Object3D�p�̒萔�ɑ���
    Object3D::UpdateCBMatViewPerse(camMPtr_);
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

void FrameWork::DebugGui(void)
{
    GUI::Begin("postEffect Settings", { 200,100 });

    static int current = 0;
    const char* shaders[] = { "NONE", "GaussianBlur", "HighLuminance", "Bloom", "RadialBlur" };
    if (GUI::DropDownTrg("Shader", &current, shaders, IM_ARRAYSIZE(shaders)))
    {
        ResetPostEffect(current);
    }
    GUI::End();
}

void FrameWork::ResetPostEffect(int num)
{
    switch (num)
    {
    case 0:
        postEffect_.reset();
        break;
    case 1:
        postEffect_ = std::make_unique<GaussianBlur>();
        break;
    case 2:
        postEffect_ = std::make_unique<HighLumi>();
        break;
    case 3:
        postEffect_ = std::make_unique<Bloom>();
        break;
    case 4:
        postEffect_ = std::make_unique<RadialBlur>();
        break;
    default:
        postEffect_ = std::make_unique<PostEffect>();
        break;
    }
    if (postEffect_.get())
        postEffect_->Initialize();
}
