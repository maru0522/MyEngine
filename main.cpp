#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<math.h>
using namespace DirectX;
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<Windows.h>
#include<vector>
#include<string>
#include<DirectXTex.h>
#include<wrl.h>
#include "Util.h"
#include <memory>

#include "WndAPI.h"
#include "Input.h"
#include "InitDirectX.h"
#include "TextureManager.h"
#include "ConstBuffer.h"
#include "GraphicsPipeline.h"
#include "CameraManager.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SceneManager.h"

using namespace Microsoft::WRL;
using BlendMode = HelperGraphicPipeline::BlendMode;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // WndAPIの変数宣言
    std::unique_ptr<WndAPI> wnd{ std::make_unique<WndAPI>() };

    // InitDirectXの変数宣言と初期化
    InitDirectX* iDXPtr{ InitDirectX::GetInstance() };
    iDXPtr->Initialize(wnd.get());

    // DirectInputに含まれるクラス全て初期化
    Input::InitializeAll(wnd.get());

    // textureManagerの変数宣言と初期化
    std::unique_ptr<TextureManager> texMPtr{ std::make_unique<TextureManager>() };
    texMPtr->Initialize();

    // GraphicsPipelineの変数宣言と初期化
    GraphicsPipeline* gplPtr{ GraphicsPipeline::GetInstance() };
    gplPtr->Initialize();

    // CameraManagerの変数宣言と初期化
    std::unique_ptr<CameraManager> cameraMPtr{ std::make_unique<CameraManager>() };
    std::unique_ptr<Camera> cameraPtr{ std::make_unique<Camera>() };
    cameraPtr->SetPosition({ 0,0,-20 });
    cameraMPtr->SetCurrentCamera(cameraPtr.get());

    // Spriteの初期化
    Sprite::StaticInitialize(texMPtr.get(), cameraMPtr.get());

    // ModelManagerの変数宣言
    std::unique_ptr<ModelManager> modelMPtr{ std::make_unique<ModelManager>(texMPtr.get()) };
    modelMPtr->LoadOBJ("Resources/cube/cube.obj");

    // テクスチャマッピング
    texMPtr->LoadFolder("Resources");

    // Object3D
    Object3D::StaticInitialize(modelMPtr.get(), texMPtr.get(), cameraMPtr.get());

    // sceneManager
    std::unique_ptr<SceneManager> sceneM{ std::make_unique<SceneManager>() };
    sceneM->Initialize(SceneFactory::Usage::DEMO);

#pragma endregion

    // ゲームループ
    while (true) {

#pragma region ウィンドウメッセージ処理

        if (!wnd->GetKeepWindow()) {
            break;
        }

#pragma endregion

#pragma region DirextX毎フレーム処理
        // DirectX毎フレーム処理　ここから
        Input::UpdateAll();
        cameraMPtr->Update();

        Object3D::UpdateCBMatViewPerse();
        sceneM->Update();

        iDXPtr->PreDraw();
        Sprite::PreDraw();
        sceneM->Draw2d();

        Object3D::PreDraw();
        sceneM->Draw3d();

        iDXPtr->PostDraw();
#pragma endregion

    }

    // ウィンドウクラスを登録解除
    wnd->Delete();

    return 0;
}
