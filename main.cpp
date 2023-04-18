﻿#include<d3d12.h>
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

using namespace Microsoft::WRL;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // WndAPIの変数宣言
    std::unique_ptr<WndAPI> wnd_{ std::make_unique<WndAPI>() };

    // InitDirectXの変数宣言と初期化
    InitDirectX* iDXPtr{ InitDirectX::GetInstance() };
    iDXPtr->Initialize(wnd_.get());

    // DirectInputに含まれるクラス全て初期化
    Input::InitializeAll(wnd_.get());

    // textureManagerの変数宣言と初期化
    std::unique_ptr<TextureManager> texMPtr{ std::make_unique<TextureManager>() };
    texMPtr->Initialize();

    // GraphicsPipelineの変数宣言と初期化
    GraphicsPipeline* gplPtr{ GraphicsPipeline::GetInstance() };
    gplPtr->Initialize();

    // CameraManagerの変数宣言と初期化
    std::unique_ptr<CameraManager> cameraMPtr{ std::make_unique<CameraManager>()};
    std::unique_ptr<Camera> cameraPtr{ std::make_unique<Camera>() };
    cameraMPtr->SetCurrentCamera(cameraPtr.get());

    // Spriteの初期化
    Sprite::StaticInitialize(texMPtr.get(), cameraMPtr.get());

    // テクスチャマッピング
    texMPtr->LoadFolder("Resources");

#pragma endregion

    Sprite sprite("Resources/namida.png");

    // ゲームループ
    while (true) {

#pragma region ウィンドウメッセージ処理

        wnd_->GetKeepWindow();

#pragma endregion

#pragma region DirextX毎フレーム処理
        // DirectX毎フレーム処理　ここから
        Input::UpdateAll();
        cameraMPtr->Update();

        sprite.Update();

        iDXPtr->PreDraw();

        Sprite::PreDraw();

        sprite.Draw();

        //commandList->SetDescriptorHeaps(1, &srvHeap);

#pragma region 構造化に伴いコメントアウト
        //// 0番定数バッファビュー(CBV)の設定コマンド
        //commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform0->GetGPUVirtualAddress());

        //// 描画コマンド
        //commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画

        //// 1番定数バッファビュー(CBV)の設定コマンド
        //commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform1->GetGPUVirtualAddress());

        //// 描画コマンド
        //commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0); // 全ての頂点を使って描画
#pragma endregion

        iDXPtr->PostDraw();
#pragma endregion

    }

    // ウィンドウクラスを登録解除
    wnd_->Delete();

    return 0;
}
