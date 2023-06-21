#pragma once
#include "Sprite.h"
#include "Vector4.h"

class PostEffect :
    public Sprite
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
    ComPtr<ID3D12Resource> texBuff_;   // texバッファ
    ComPtr<ID3D12Resource> depthBuff_; // 深度バッファ
    ComPtr<ID3D12DescriptorHeap> srvHeap_; // srv用デスクヒープ
    ComPtr<ID3D12DescriptorHeap> rtvHeap_; // rtv用デスクヒープ
    ComPtr<ID3D12DescriptorHeap> dsvHeap_; // dsv用デスクヒープ
};

