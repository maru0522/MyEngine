#pragma once
#include "InitDirectX.h"
#include <cassert>
#include <d3d12.h>
#include <vector>
#include "Util.h"
#include <wrl.h>

class IndexBuffer
{
public:
    // 関数
    IndexBuffer(const std::vector<unsigned short>& indices) {
        // インデックスデータ全体のサイズ
        unsigned int sizeIB = static_cast<unsigned int>(sizeof(unsigned short) * indices.size());

        // インデックスバッファの設定
        D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

        // リソース設定
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = sizeIB; // インデックス情報が入る分のサイズ
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // インデックスバッファの生成
        HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateCommittedResource(
            &heapProp, // ヒープ設定
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // リソース設定
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&buff_));
        assert(SUCCEEDED(hr));

        // マッピング
        unsigned short* indexMap_{ nullptr }; // GPUメモリのmap
        hr = buff_->Map(0, nullptr, (void**)&indexMap_);
        assert(SUCCEEDED(hr));

        std::copy(indices.begin(), indices.end(), indexMap_); // 全インデックスに対して

        // マッピング解除
        buff_->Unmap(0, nullptr);

        // インデクスバッファビュー
        // GPU仮想アドレス
        ibView_.BufferLocation = buff_->GetGPUVirtualAddress();
        ibView_.Format = DXGI_FORMAT_R16_UINT;
        // インデックスバッファのサイズ
        ibView_.SizeInBytes = sizeIB;
    }

    ~IndexBuffer(void) {}

    inline ID3D12Resource* GetBuffer(void) { return buff_.Get(); }
    inline const D3D12_INDEX_BUFFER_VIEW& GetIbView(void) { return ibView_; }
    inline size_t GetIndicesNum(void) { return buff_->GetDesc().Width / sizeof(unsigned short); }

private:
    // 変数
    Microsoft::WRL::ComPtr<ID3D12Resource> buff_{ nullptr }; // 頂点バッファ
    D3D12_INDEX_BUFFER_VIEW ibView_{}; // 頂点バッファビュー
};