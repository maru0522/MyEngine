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
    // InitDirectX初期化
    iDXPtr_->Initialize(wnd_.get());
    // 入力系クラス全て初期化
    Input::InitializeAll(wnd_.get());
    // TextureManager初期化
    texM_->Initialize();
    // TextureManager - テクスチャ読み込み
    texM_->LoadFolder("Resources");
    // ModelManager - 3Dモデル読み込み
    modelM_->LoadOBJ("Resources/model/cube/cube.obj", false);
    modelM_->LoadOBJ("Resources/model/ICOSphere/ICOSphere.obj", false);
    modelM_->LoadOBJ("Resources/model/lolP/lolP.obj", false);
    //modelM_->LoadOBJ("Resources/model/player/Casual_Male.obj", false);
    modelM_->LoadOBJ("Resources/model/sphere/sphere.obj", true);
    modelM_->LoadOBJ("Resources/model/pipe/pipe.obj", false);
    modelM_->LoadOBJ("Resources/model/coin/coin.obj", false);
    modelM_->LoadOBJ("Resources/model/skydome/skydome.obj",true);
    modelM_->LoadOBJ("Resources/model/character/chr_sword.obj",false);
    modelM_->LoadOBJ("Resources/model/rabbit/rabbit.obj",false);
    // AudioManager - 音楽&SE読み込み
    audioM_->LoadFolder("Resources/sound");
    // GraphicsPipeline初期化
    psoMPtr_->Create();

    // Sprite初期化
    Sprite::StaticInitialize(texM_.get());
    // Object3D初期化
    Object3D::StaticInitialize(modelM_.get(), texM_.get());
    // Sound初期化
    Sound::StaticInitalize(audioM_.get());

    // SceneManager初期化
    SceneManager::GetInstance()->Initialize(SceneName::TITLE);
    // ImguiController初期化
    imguiController_->Initialize(wnd_.get());
}

void FrameWork::Update(void)
{
    // 入力系クラス全て更新
    Input::UpdateAll();

    // ImguiController更新開始
    imguiController_->Begin();

    // SceneManager更新
    SceneManager::GetInstance()->Update();
    DebugGui();

    // ImguiController更新終了
    imguiController_->End();

    // CameraManager更新
    camMPtr_->Update();

    // カメラの透視投影行列をObject3D用の定数に送る
    Object3D::UpdateCBMatViewPerse(camMPtr_);
}

void FrameWork::Finalize(void)
{
    // ウィンドウクラスを登録解除
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
