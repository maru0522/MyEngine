#pragma once
#include "Sprite.h"

class PostEffect :
    public Sprite
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    // �֐�
    PostEffect(void);
    void Initialize(void);
    void Draw(void);

    // �ϐ�
    ComPtr<ID3D12Resource> texBuff_;
    ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

