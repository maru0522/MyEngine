#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Vector4.h"
#include "RenderTexture.h"

class PostEffect
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    
    // 定数
    static const float kClearColor[4];

public:
    // 関数
    PostEffect(void);
    void Initialize(void);
    void PreDrawScene(void);
    void Draw(void);
    void PostDrawScene(void);

    // 変数
    Matrix4 matWorld_;

    ConstBuffer<Sprite::CBData2d_t> cb_;
    VertexBuffer<Sprite::VertexPosUv_t> vertexBuffer_;

    RenderTexture renderTexture_[2];
    ComPtr<ID3D12Resource> depthBuff_;  // 深度バッファ
    ComPtr<ID3D12DescriptorHeap> srvHeap_; // srv用デスクヒープ
    ComPtr<ID3D12DescriptorHeap> rtvHeap_; // rtv用デスクヒープ
    ComPtr<ID3D12DescriptorHeap> dsvHeap_; // dsv用デスクヒープ
};

