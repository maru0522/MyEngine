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
    // InitDirectX初期化
    iDXPtr_->Initialize(wnd_.get());
    // 入力系クラス全て初期化
    Input::InitializeAll(wnd_.get());
    // TextureManager初期化
    texM_->Initialize();
    // GraphicsPipeline初期化
    gplPtr_->Initialize();

    // Sprite初期化
    Sprite::StaticInitialize(texM_.get());
    // ModelManager - 3Dモデル読み込み
    modelM_->LoadOBJ("Resources/cube/cube.obj");
    // TextureManager - テクスチャ読み込み
    texM_->LoadFolder("Resources");
    // Object3D初期化
    Object3D::StaticInitialize(modelM_.get(), texM_.get());

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
