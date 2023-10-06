#pragma once
#include <wrl.h>
#include <d3d12.h>

class RenderTexture final
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    // 定数
    static const float kClearColor[4];

public:
    // 関数
    RenderTexture(void) = default;
    void Create(void);
    void TransitionResourceBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

private:
    // 変数
    ComPtr<ID3D12Resource> texBuff_;   // texバッファ

public:
    // setter・getter
    inline ID3D12Resource* GetTexBuffPtr(void) { return texBuff_.Get(); }
};

