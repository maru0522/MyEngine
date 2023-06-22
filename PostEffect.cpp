#include <d3dx12.h>
#include "WndAPI.h"
#include "PostEffect.h"
#include "InitDirectX.h"

const float  PostEffect::kClearColor[4]{ 0.25f, 0.5f, 0.1f, 0.f }; // 黄緑みたいな

PostEffect::PostEffect(void) 
{
    // 頂点バッファ
    std::vector<Sprite::VertexPosUv_t> vertices;
    vertices.emplace_back(Sprite::VertexPosUv_t{{ -1.f, -1.f, 0.0f }, {0.0f, 1.0f} }); // 左下
    vertices.emplace_back(Sprite::VertexPosUv_t{{ -1.f, +1.f, 0.0f }, {0.0f, 0.0f} }); // 左上
    vertices.emplace_back(Sprite::VertexPosUv_t{{ +1.f, -1.f, 0.0f }, {1.0f, 1.0f} }); // 右下
    vertices.emplace_back(Sprite::VertexPosUv_t{{ +1.f, +1.f, 0.0f }, {1.0f, 0.0f} }); // 右上
    vertexBuffer_.Create(vertices);

    // 定数バッファ
    cb_.Create();
    cb_.GetConstBuffMap()->matWorld = Math::Matrix::Identity();
    cb_.GetConstBuffMap()->color = { 1.f,1.f,1.f,1.f };
}

void PostEffect::Initialize(void)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // テクスチャヒープ設定
    CD3DX12_HEAP_PROPERTIES textureHeapProp{ D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0 };
    // テクスチャリソース設定
    CD3DX12_RESOURCE_DESC textureDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, WndAPI::kWidth_, (UINT)WndAPI::kHeight_, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    // テクスチャクリア設定
    CD3DX12_CLEAR_VALUE texClearValue{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kClearColor };
    // テクスチャバッファの生成
    HRESULT hr = iDXPtr->GetDevice()->
        CreateCommittedResource(&textureHeapProp, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &texClearValue, IID_PPV_ARGS(&texBuff_));
    assert(SUCCEEDED(hr));

    // テクスチャを赤クリア
    constexpr uint32_t pixelCount = WndAPI::kWidth_ * WndAPI::kHeight_; // 画素数
    constexpr uint32_t rowPitch = sizeof(uint32_t) * WndAPI::kWidth_;   // 1行分のデータサイズ
    constexpr uint32_t deothPitch = rowPitch * WndAPI::kHeight_;        // 全体のデータサイズ
    // イメージ
    std::vector<uint32_t> img(pixelCount);
    for (size_t i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

    // テクスチャバッファにデータ転送
    hr = texBuff_->WriteToSubresource(0, nullptr, img.data(), rowPitch, deothPitch);
    assert(SUCCEEDED(hr));


    // srv用descHeap設定
    D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc{};
    srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvDescHeapDesc.NumDescriptors = 1;
    // srv用descHeap生成
    hr = iDXPtr->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&srvHeap_));
    assert(SUCCEEDED(hr));

    // srv設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    // descHeapにsrv生成
    iDXPtr->GetDevice()->CreateShaderResourceView(texBuff_.Get(), &srvDesc, srvHeap_->GetCPUDescriptorHandleForHeapStart());
    //iDXPtr->GetDescHeap_t()->CreateSRV(textureDesc, texBuff_.Get());


    // rtv用デスクヒープ設定
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
    rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDescHeapDesc.NumDescriptors = 1;
    // rtv用デスクヒープ生成
    hr = iDXPtr->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&rtvHeap_));
    assert(SUCCEEDED(hr));

    // rtv設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    // シェーダーの計算結果をSRGBに変換して書き込む
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    // デスクヒープにrtv生成
    iDXPtr->GetDevice()->CreateRenderTargetView(texBuff_.Get(), &rtvDesc, rtvHeap_->GetCPUDescriptorHandleForHeapStart());


    // 深度バッファヒープ設定
    CD3DX12_HEAP_PROPERTIES depthHeapProp{ D3D12_HEAP_TYPE_DEFAULT };
    // 深度バッファリソース設定
    CD3DX12_RESOURCE_DESC depthDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WndAPI::kWidth_, WndAPI::kHeight_, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    // 深度バッファクリア設定
    CD3DX12_CLEAR_VALUE depthClearValue{ DXGI_FORMAT_D32_FLOAT,1.f,0 };
    // 深度バッファ生成
    hr = iDXPtr->GetDevice()->CreateCommittedResource(
        &depthHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&depthBuff_));
    assert(SUCCEEDED(hr));


    // dsv用デスクヒープ設定
    D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc{};
    dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvDescHeapDesc.NumDescriptors = 1;
    // dsv用デスクヒープ生成
    hr = iDXPtr->GetDevice()->CreateDescriptorHeap(&dsvDescHeapDesc, IID_PPV_ARGS(&dsvHeap_));
    assert(SUCCEEDED(hr));
    // デスクヒープにdsv生成
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    iDXPtr->GetDevice()->CreateDepthStencilView(depthBuff_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::PreDrawScene(void)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // リソースバリア変更（シェーダーリソース->描画可能）
    CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    iDXPtr->GetCommandList()->ResourceBarrier(1, &resBarrier);

    // rtv用デスクヒープのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    // dsv用デスクヒープのハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
    // rtvセット
    iDXPtr->GetCommandList()->OMSetRenderTargets(1, &rtvH, false, &dsvH);

    // ビューポート設定
    CD3DX12_VIEWPORT viewPort{ 0.f,0.f,WndAPI::kWidth_,WndAPI::kHeight_ };
    iDXPtr->GetCommandList()->RSSetViewports(1, &viewPort);
    // シザリング矩形設定
    CD3DX12_RECT scissorRect{ 0,0,WndAPI::kWidth_,WndAPI::kHeight_ };
    iDXPtr->GetCommandList()->RSSetScissorRects(1, &scissorRect);

    // 全画面クリア
    iDXPtr->GetCommandList()->ClearRenderTargetView(rtvH, kClearColor, 0, nullptr);
    // 深度バッファクリア
    iDXPtr->GetCommandList()->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void PostEffect::Draw(void)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // プリミティブ形状の設定コマンド
    iDXPtr->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト

    // SRVヒープの設定コマンド
    //std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDXPtr->GetDescHeap_t()->GetDescHeap() };
    std::vector<ID3D12DescriptorHeap*> ppHeaps = { srvHeap_.Get() };
    iDXPtr->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // パイプラインステートとルートシグネチャの設定コマンド
    //ID3D12PipelineState* plsPtr{ GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).pipelineState.Get() };
    iDXPtr->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSO("PSO_POSTEFFECT", GraphicsPipeline::BlendMode::NONE)->pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("PSO_POSTEFFECT", GraphicsPipeline::BlendMode::NONE)->rootSignature.Get());

    // 頂点バッファビューの設定コマンド
    iDXPtr->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVbView());

    // 定数バッファビュー(CBV)の設定コマンド
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());

    // SRVヒープの先頭にあるSRVをルートパラメータ0番に設定
    //iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, imagePtr_->srvGpuHandle);
    iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHeap_->GetGPUDescriptorHandleForHeapStart());

    // 描画コマンド
    iDXPtr->GetCommandList()->DrawInstanced((unsigned int)vertexBuffer_.GetVerticesNum(), 1, 0, 0); // 全ての頂点を使って描画
}

void PostEffect::PostDrawScene(void)
{
    // リソースバリア変更（描画可能->シェーダーリソース）
    CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    InitDirectX::GetInstance()->GetCommandList()->ResourceBarrier(1, &resBarrier);
}
