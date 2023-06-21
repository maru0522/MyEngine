#pragma once
#include "Sprite.h"

class PostEffect :
    public Sprite
{
private:
    // ’è‹`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    // ŠÖ”
    PostEffect(void);
    void Initialize(void);
    void Draw(void);

    // •Ï”
    ComPtr<ID3D12Resource> texBuff_;
    ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

