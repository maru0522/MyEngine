#pragma once
#include "Sprite.h"

class PostEffect :
    public Sprite
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    // 関数
    PostEffect(void);
    void Initialize(void);
    void Draw(void);

    // 変数
    ComPtr<ID3D12Resource> texBuff_;
    ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

