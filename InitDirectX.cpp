#include <cassert>
#include <vector>

#include "InitDirectX.h"

InitDirectX* InitDirectX::GetInstance(void)
{
    static InitDirectX iDX;
    return &iDX;
}

void InitDirectX::Initialize(WndAPI* p_wndapi)
{
    // FPS固定化処理初期化
    fpsController_->Initialize();

    DebugLayer();       // デバッグレイヤーをオン
#ifdef _DEBUG
    DXGIDevice();
    SuppressErrors();   // 一部のエラーを抑制
#endif
    Commands();
    SwapChain(p_wndapi);
    RTVDescHeap();
    RTV();
    DescriptorHeap();
    DepthBuffer();
    Fence();
}

void InitDirectX::PreDraw(void)
{
#pragma region リソースバリアの変更
    // バックバッファの番号を取得（2つなので0番か1番）
    UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

    // １．リソースバリアで書き込み可能に変更
    D3D12_RESOURCE_BARRIER barrierDesc{};
    barrierDesc.Transition.pResource = backBuffers_[bbIndex].Get(); // バックバッファを指定
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 表示状態から
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態へ
    commandList_->ResourceBarrier(1, &barrierDesc);
#pragma endregion

#pragma region 描画先の変更
    // ２．描画先の変更
    // レンダーターゲットビューのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvDescHeapDesc_.Type);

    // 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

    // レンダーターゲットをセット
    commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
#pragma endregion

#pragma region 画面クリア
    // ３．画面クリア 
    ClearRTV();

    // 深度バッファクリア
    ClearDepthBuff();
#pragma endregion

#pragma region グラフィックスコマンド入力前設定
    // ４．描画コマンドここから
    // ビューポート設定コマンド
    D3D12_VIEWPORT viewport{};
    viewport.Width = WndAPI::kWidth_;
    viewport.Height = WndAPI::kHeight_;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    // ビューポート設定コマンドを、コマンドリストに積む
    commandList_->RSSetViewports(1, &viewport);

    // シザー矩形
    D3D12_RECT scissorRect{};
    scissorRect.left = 0; // 切り抜き座標左
    scissorRect.right = scissorRect.left + WndAPI::kWidth_; // 切り抜き座標右
    scissorRect.top = 0; // 切り抜き座標上
    scissorRect.bottom = scissorRect.top + WndAPI::kHeight_; // 切り抜き座標下
    // シザー矩形設定コマンドを、コマンドリストに積む
    commandList_->RSSetScissorRects(1, &scissorRect);
#pragma endregion
    // ----- ↓↓↓PreDraw()とPostDraw()の間に描画コマンドを書き込む↓↓↓ ----- //
}

void InitDirectX::PostDraw(void)
{
    // ----- ↑↑↑PreDraw()とPostDraw()の間に描画コマンドを書き込む↑↑↑ ----- //
    // ４．描画コマンドここまで

    // バックバッファの番号を取得（2つなので0番か1番）
    UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

#pragma region リソースバリアを元に戻す
    // ５．リソースバリアを戻す
    D3D12_RESOURCE_BARRIER barrierDesc{};
    barrierDesc.Transition.pResource = backBuffers_[bbIndex].Get(); // バックバッファを指定
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 描画状態から
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 表示状態へ
    commandList_->ResourceBarrier(1, &barrierDesc);
#pragma endregion

#pragma region コマンドリスト実行
    // 命令のクローズ
    HRESULT hr = commandList_->Close();
    assert(SUCCEEDED(hr));

    // コマンドリストの実行
    std::vector<ID3D12CommandList*> commandLists = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, commandLists.data());
    // 画面に表示するバッファをフリップ（裏表の入替え）
    hr = swapChain_->Present(1, 0);
    assert(SUCCEEDED(hr));

    // コマンドの実行完了を待つ
    commandQueue_->Signal(fence_.Get(), ++fenceVal_);
    if (fence_->GetCompletedValue() != fenceVal_) {
        HANDLE event = CreateEvent(nullptr, false, false, nullptr);
        if (event != 0) {
            fence_->SetEventOnCompletion(fenceVal_, event);
            WaitForSingleObject(event, INFINITE);
            CloseHandle(event);
        }
    }

    // FPS固定
    fpsController_->Update();

    // キューをクリア
    hr = cmdAllocator_->Reset();
    assert(SUCCEEDED(hr));


    // 再びコマンドリストを貯める準備
    hr = commandList_->Reset(cmdAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr));
#pragma endregion
}

void InitDirectX::DebugLayer(void)
{
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
        debugController_->EnableDebugLayer();
        debugController_->SetEnableGPUBasedValidation(true);
    }
}

void InitDirectX::DXGIDevice(void)
{
#pragma region アダプタの列挙
    // DXGIファクトリーの生成
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

    assert(SUCCEEDED(hr));

    // アダプターの列挙用
    std::vector<ComPtr<IDXGIAdapter4>> adapters;

    // ここに特定の名前を持つアダプターオブジェクトが入る
    ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

    // パフォーマンスが高いものから順に、全てのアダプターを列挙する
    for (UINT i = 0;
        dxgiFactory_->EnumAdapterByGpuPreference( // ここの引数がUINTのため i の型がsize_tでない
            i,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
        i++) {

        // 動的配列に追加する
        adapters.push_back(tmpAdapter);
    }
#pragma endregion

#pragma region アダプタの選別
    // 妥当なアダプタを選別する
    for (size_t i = 0; i < adapters.size(); i++) {
        DXGI_ADAPTER_DESC3 adapterDesc;

        // アダプターの情報を取得する
        adapters.at(i)->GetDesc3(&adapterDesc);

        // ソフトウェアデバイスを回避
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // デバイスを採用してループを抜ける
            tmpAdapter = adapters.at(i);

            break;
        }
    }
#pragma endregion

#pragma region デバイスの生成
    // 対応レベルの配列
    std::vector<D3D_FEATURE_LEVEL> levels{
     D3D_FEATURE_LEVEL_12_1,
     D3D_FEATURE_LEVEL_12_0,
     D3D_FEATURE_LEVEL_11_1,
     D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;
    for (size_t i = 0; i < levels.size(); i++) {
        // 採用したアダプターでデバイスを生成
        hr = D3D12CreateDevice(tmpAdapter.Get(), levels.at(i), IID_PPV_ARGS(&device_));

        if (hr == S_OK) {
            // デバイスを生成できた時点でループを抜ける
            featureLevel = levels.at(i);
            break;
        }
    }
#pragma endregion
}

void InitDirectX::SuppressErrors(void)
{
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
        infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);    //やばいエラーの時止まる
        infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);        //エラーの時止まる
    }

    //抑制するエラー 
    std::vector<D3D12_MESSAGE_ID> denyIds = {
        D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
    };

    //抑制される表示レベル
    std::vector<D3D12_MESSAGE_SEVERITY> severities = { D3D12_MESSAGE_SEVERITY_INFO };
    D3D12_INFO_QUEUE_FILTER filter{};
    filter.DenyList.NumIDs = (UINT)denyIds.size();
    filter.DenyList.pIDList = denyIds.data();
    filter.DenyList.NumSeverities = (UINT)severities.size();
    filter.DenyList.pSeverityList = severities.data();
    //指定したエラーの表示を抑制する
    infoQueue_->PushStorageFilter(&filter);
}

void InitDirectX::Commands(void)
{
#pragma region コマンドアロケータ
    // コマンドアロケータを生成
    HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator_));

    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region コマンドリストの生成
    // コマンドリストを生成
    hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));

    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region コマンドキューの生成
    //コマンドキューの設定
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

    //コマンドキューを生成
    hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

    assert(SUCCEEDED(hr));
#pragma endregion
}

void InitDirectX::SwapChain(WndAPI* p_wndapi)
{
    // スワップチェーンの設定
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = 1280;
    swapChainDesc.Height = 720;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
    swapChainDesc.SampleDesc.Count = 1; // マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
    swapChainDesc.BufferCount = 2; // バッファ数を２つに設定
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // IDXGISwapChain1のtmp
    ComPtr<IDXGISwapChain1> swapChain1Tmp;

    // スワップチェーンの生成
    HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), p_wndapi->GetHwnd(), &swapChainDesc, nullptr, nullptr, &swapChain1Tmp);
    assert(SUCCEEDED(hr));

    // 生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
    swapChain1Tmp.As(&swapChain_);
}

void InitDirectX::RTVDescHeap(void)
{
    // swapChainDesc1 の 空変数を宣言
    DXGI_SWAP_CHAIN_DESC swcd{};

    // コピーで取得
    HRESULT hr = swapChain_->GetDesc(&swcd);
    assert(SUCCEEDED(hr));

    // RTVDescHeap
    // デスクリプタヒープの設定
    rtvDescHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
    rtvDescHeapDesc_.NumDescriptors = swcd.BufferCount; // 裏表の２つ

    // 設定を元にRTV用デスクリプタヒープの生成
    device_->CreateDescriptorHeap(&rtvDescHeapDesc_, IID_PPV_ARGS(&rtvHeap_));
}

void InitDirectX::RTV(void)
{
    // swapChainDesc1 の 空変数を宣言
    DXGI_SWAP_CHAIN_DESC swcd{};

    // コピーで取得
    HRESULT hr = swapChain_->GetDesc(&swcd);
    assert(SUCCEEDED(hr));

    // RTV
    backBuffers_.resize(swcd.BufferCount);

    // スワップチェーンの全てのバッファについて処理する
    for (size_t i = 0; i < backBuffers_.size(); i++) {
        // スワップチェーンからバッファを取得
        swapChain_->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&backBuffers_.at(i)));
        // デスクリプタヒープのハンドルを取得
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
        // 裏か表かでアドレスがずれる
        rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvDescHeapDesc_.Type);
        // レンダーターゲットビューの設定
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        // シェーダーの計算結果をSRGBに変換して書き込む
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        // レンダーターゲットビューの生成
        device_->CreateRenderTargetView(backBuffers_.at(i).Get(), &rtvDesc, rtvHandle);
    }
}

void InitDirectX::DescriptorHeap(void)
{
    descHeap_ = std::make_unique<DescriptorHeap_t>(this);
}

void InitDirectX::DepthBuffer(void)
{
    //深度テスト深度バッファ

    // リソース設定
    D3D12_RESOURCE_DESC depthResourceDesc{};
    depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthResourceDesc.Width = WndAPI::kWidth_;     // レンダーターゲットに合わせる
    depthResourceDesc.Height = WndAPI::kHeight_;   // レンダーターゲットに合わせる
    depthResourceDesc.DepthOrArraySize = 1;
    depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;   // 深度値フォーマット
    depthResourceDesc.SampleDesc.Count = 1;
    depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;  // デプスステンシル

    // 深度値用ヒーププロパティ
    D3D12_HEAP_PROPERTIES depthHeapProp{};
    depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;      // 深度値1.0f（最大値）でクリア
    depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; // 深度値フォーマット

    // 深度バッファの生成
    // リソース生成
    HRESULT hr = device_->CreateCommittedResource(
        &depthHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &depthResourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,       // 深度値書き込みに使用
        &depthClearValue,
        IID_PPV_ARGS(&depthBuff_)
    );
    assert(SUCCEEDED(hr));

    // 深度ビュー用デスクリプタヒープの設定
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;     // 深度ビューは1つ
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;      // デプスステンシルビュー
    // 設定を元に深度ビュー用デスクリプタヒープの生成
    hr = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
    assert(SUCCEEDED(hr));

    // 深度ビュー作成
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;     // 深度値フォーマット
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    // 深度ビューの生成
    device_->CreateDepthStencilView(depthBuff_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
}

void InitDirectX::Fence(void)
{
    // フェンスの生成
    HRESULT hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));
}

void InitDirectX::ClearRTV(void)
{
    // バックバッファの番号を取得（2つなので0番か1番）
    UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

    // レンダーターゲットビューのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvDescHeapDesc_.Type);

    // 何色でクリアするのか設定
    std::vector<FLOAT> clearColor = { 0.1f,0.25f,0.5f,0.f };
    // RTVをクリアする関数をコマンドリストに積む
    commandList_->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);
}

void InitDirectX::ClearDepthBuff(void)
{
    // 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

    // 深度バッファをクリアする関数をコマンドリストに積む
    commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

InitDirectX::DescriptorHeap_t::DescriptorHeap_t(InitDirectX* p_idx)
    : p_idx_(p_idx)
{
    // デスクリプタヒープの設定
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// シェーダから見えるように
    descHeapDesc.NumDescriptors = maxSRVDesc_ + maxCBVDesc_ + maxUAVDesc_;

    // 設定を元にSRV用デスクリプタヒープを作成
    HRESULT hr = p_idx_->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
    assert(SUCCEEDED(hr));
}

size_t InitDirectX::DescriptorHeap_t::CreateSRV(const D3D12_RESOURCE_DESC& rscDesc, ID3D12Resource* p_rsc)
{
    assert(SRVCount_ < maxSRVDesc_);

    // デスクリプタのサイズ
    size_t incrementSize = p_idx_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // cpuのdescriptorのヒープのスタート位置
    D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_{ descriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart() };
    // ハンドルを進める
    srvCpuHandle_.ptr += incrementSize * SRVCount_;

    // gpuのdescriptorのヒープのスタート位置
    D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_{ descriptorHeap_.Get()->GetGPUDescriptorHandleForHeapStart() };
    // ハンドルを進める
    srvGpuHandle_.ptr += incrementSize * SRVCount_;


    // シェーダリソースビュー設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = rscDesc.Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = rscDesc.MipLevels;

    // ハンドルのさす位置にシェーダーリソースビューの作成
    p_idx_->GetDevice()->CreateShaderResourceView(p_rsc, &srvDesc, srvCpuHandle_);

    // SRVを生成した数を記録
    SRVCount_++;

    return srvGpuHandle_.ptr;
}
