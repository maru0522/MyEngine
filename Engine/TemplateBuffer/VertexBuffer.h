#pragma once
#include "InitDirectX.h"
#include <cassert>
#pragma warning(push)
#pragma warning(disable:4820)
#include <d3d12.h>
#pragma warning(pop)
#include <vector>
#include "Util.h"
#include <wrl.h>

template<class T> class VertexBuffer
{
public:
    // 関数
    VertexBuffer(void) = default;
    ~VertexBuffer(void) {/* buff_->Unmap(0, nullptr); */}

    void Create(const std::vector<T>& vertices) {
        // 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
        unsigned int sizeVB = static_cast<uint32_t>(sizeof(vertices[0]) * vertices.size());

        // 頂点バッファの設定
        D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

        // リソース設定
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = sizeVB; // 頂点データ全体のサイズ
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // 頂点バッファの生成
        HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateCommittedResource(
            &heapProp, // ヒープ設定
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // リソース設定
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&buff_));
        assert(SUCCEEDED(hr));

        // GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
        hr = buff_->Map(0, nullptr, (void**)&vertMap_);
        assert(SUCCEEDED(hr));

        // 全頂点に対して
        TransferVertexToBuffer(vertices);

        // 頂点バッファビュー
        // GPU仮想アドレス
        vbView_.BufferLocation = buff_->GetGPUVirtualAddress();
        // 頂点バッファのサイズ
        vbView_.SizeInBytes = sizeVB;
        // 頂点1つ分のデータサイズ
        vbView_.StrideInBytes = sizeof(vertices[0]);
    }
    void TransferVertexToBuffer(const std::vector<T>& vertices) {
        // buffer作成時より、大きいsizeのverticesを使用時に例外スローないし不具合が起きる可能性あり。
        std::copy(vertices.begin(), vertices.end(), vertMap_); // 全頂点に対して
    }

private:
    // 変数
    Microsoft::WRL::ComPtr<ID3D12Resource> buff_{}; // 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vbView_{};             // 頂点バッファビュー
    T* vertMap_{};                                  // GPUメモリのmap

public:
    // getter
    inline ID3D12Resource* GetBuffer(void) { return buff_.Get(); }
    inline const D3D12_VERTEX_BUFFER_VIEW& GetVbView(void) { return vbView_; }
    inline size_t GetVerticesNum(void) { return buff_->GetDesc().Width / sizeof(T); }
};
