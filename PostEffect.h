#pragma once
#include <wrl.h>
#include <string>
#include <d3d12.h>
#include "Sprite.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "ConstBuffer.h"
#include "VertexBuffer.h"
#include "RenderTexture.h"

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
    virtual ~PostEffect() = default;

    virtual void Initialize(void);
    virtual void PreDrawScene(void);
    virtual void PostDrawScene(void);
    virtual void Draw();

protected:
    inline void SetPSOName(const std::string psoName) { psoName_ = psoName; }

private:
    // �ϐ�
    Matrix4 matWorld_;

    ConstBuffer<Sprite::CBData2d_t> cb_;
    VertexBuffer<Sprite::VertexPosUv_t> vertexBuffer_;

    RenderTexture renderTexture_[2];
    ComPtr<ID3D12Resource> depthBuff_;  // �[�x�o�b�t�@
    ComPtr<ID3D12DescriptorHeap> srvHeap_; // srv�p�f�X�N�q�[�v
    ComPtr<ID3D12DescriptorHeap> rtvHeap_; // rtv�p�f�X�N�q�[�v
    ComPtr<ID3D12DescriptorHeap> dsvHeap_; // dsv�p�f�X�N�q�[�v

    std::string psoName_;
};

