#pragma once
#include "Sprite.h"
#include "Vector4.h"

class PostEffect
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    
    // �萔
    static const float kClearColor[4];

public:
    // �֐�
    PostEffect(void);
    void Initialize(void);
    void PreDrawScene(void);
    void Draw(void);
    void PostDrawScene(void);

    // �ϐ�
    Matrix4 matWorld_;

    ConstBuffer<Sprite::CBData2d_t> cb_;
    VertexBuffer<Sprite::VertexPosUv_t> vertexBuffer_;

    ComPtr<ID3D12Resource> texBuff_;   // tex�o�b�t�@
    ComPtr<ID3D12Resource> depthBuff_; // �[�x�o�b�t�@
    ComPtr<ID3D12DescriptorHeap> srvHeap_; // srv�p�f�X�N�q�[�v
    ComPtr<ID3D12DescriptorHeap> rtvHeap_; // rtv�p�f�X�N�q�[�v
    ComPtr<ID3D12DescriptorHeap> dsvHeap_; // dsv�p�f�X�N�q�[�v
};

