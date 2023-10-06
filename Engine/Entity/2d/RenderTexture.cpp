#include <cassert>
#include <d3dx12.h>
#include "WndAPI.h"
#include "InitDirectX.h"
#include "RenderTexture.h"

const float  RenderTexture::kClearColor[4]{ 0.25f, 0.5f, 0.1f, 0.f }; // 黄緑みたいな

void RenderTexture::Create(void)
{
    // テクスチャヒープ設定
    CD3DX12_HEAP_PROPERTIES textureHeapProp{ D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0 };
    // テクスチャリソース設定
    CD3DX12_RESOURCE_DESC textureDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, WndAPI::kWidth_, (UINT)WndAPI::kHeight_, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    // テクスチャクリア設定
    CD3DX12_CLEAR_VALUE texClearValue{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kClearColor };
    // テクスチャバッファの生成
    HRESULT hr = InitDirectX::GetInstance()->GetDevice()->
        CreateCommittedResource(&textureHeapProp, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &texClearValue, IID_PPV_ARGS(&texBuff_));
    assert(SUCCEEDED(hr));

    // テクスチャを赤クリア
    constexpr uint32_t pixelCount = WndAPI::kWidth_ * WndAPI::kHeight_; // 画素数
    constexpr uint32_t rowPitch = sizeof(uint32_t) * WndAPI::kWidth_;   // 1行分のデータサイズ
    constexpr uint32_t deothPitch = rowPitch * WndAPI::kHeight_;        // 全体のデータサイズ
    // イメージ
    std::vector<uint32_t> img(pixelCount);
    for (size_t i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; } // 赤色

    // テクスチャバッファにデータ転送
    hr = texBuff_->WriteToSubresource(0, nullptr, img.data(), rowPitch, deothPitch);
    assert(SUCCEEDED(hr));
}

void RenderTexture::TransitionResourceBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
    // リソースバリア変更
    CD3DX12_RESOURCE_BARRIER cResBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), stateBefore, stateAfter);
    InitDirectX::GetInstance()->GetCommandList()->ResourceBarrier(1, &cResBarrier);
}
