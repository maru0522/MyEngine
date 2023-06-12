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
    // InitDirectX初期化
    iDXPtr_->Initialize(wnd_.get());
    // 入力系クラス全て初期化
    Input::InitializeAll(wnd_.get());
    // TextureManager初期化
    texM_->Initialize();
    // TextureManager - テクスチャ読み込み
    texM_->LoadFolder("Resources");
    // ModelManager - 3Dモデル読み込み
    modelM_->LoadOBJ("Resources/model/cube/cube.obj",false);
    modelM_->LoadOBJ("Resources/model/ICOSphere/ICOSphere.obj",false);
    // AudioManager - 音楽&SE読み込み
    audioM_->LoadFolder("Resources/sound");
    // GraphicsPipeline初期化
    gplPtr_->Initialize();

    // Sprite初期化
    Sprite::StaticInitialize(texM_.get());
    // Object3D初期化
    Object3D::StaticInitialize(modelM_.get(), texM_.get());
    // Sound初期化
    Sound::StaticInitalize(audioM_.get());

    // SceneManager初期化
    sceneM_->Initialize(SceneFactory::Usage::DEMO);
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
    sceneM_->Update();

    // ImguiController更新終了
    imguiController_->End();

    // CameraManager更新
    CameraManager::GetInstance()->Update();

    // カメラの透視投影行列をObject3D用の定数に送る
    Object3D::UpdateCBMatViewPerse();
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
