#pragma once
#include <wrl.h>
#include <d3d12.h>

class RenderTexture final
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    // �萔
    static const float kClearColor[4];

public:
    // �֐�
    RenderTexture(void) = default;
    void Create(void);
    void TransitionResourceBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

private:
    // �ϐ�
    ComPtr<ID3D12Resource> texBuff_;   // tex�o�b�t�@

public:
    // setter�Egetter
    inline ID3D12Resource* GetTexBuffPtr(void) { return texBuff_.Get(); }
};

