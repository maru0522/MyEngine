#pragma once
#include <vector>
#include <wrl.h>
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "WndAPI.h"
#include "FPSController.h"

class InitDirectX
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct DescriptorHeap_t
    {
    public:
        // 関数
        DescriptorHeap_t(InitDirectX* p_idx);

        // srvをヒープに生成
        size_t CreateSRV(const D3D12_RESOURCE_DESC& rscDesc, ID3D12Resource* p_rsc);

        // setter
        inline void SetMaxSrvDesc(uint32_t maxSrv) { maxSRVDesc = maxSrv; }

        // getter
        inline ID3D12DescriptorHeap* GetDescHeap(void) { return descriptorHeap_.Get(); }

    private:
        // 変数
        // DescriptorHeapコンストラクタ で初期化
        ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
        uint32_t maxSRVDesc{ 256 };
        uint32_t maxCBVDesc{ 256 };
        uint32_t maxUAVDesc{ 256 };
        uint32_t SRVCount{ 1 }; // ImGui用のデスクリプタ生成のため1つ分あける。
        uint32_t CBVCount{};
        uint32_t UAVCount{};

        InitDirectX* iDXPtr{ nullptr };
    };

public:
    // 静的関数
    static InitDirectX* GetInstance(void);

    // 関数
    void Initialize(WndAPI* p_wndapi);
    void PreDraw(void);
    void PostDraw(void);

    // getter
    inline ID3D12Device* GetDevice(void) { return device_.Get(); }
    inline DescriptorHeap_t* GetDescHeap_t(void) { return descHeap_.get(); }
    inline ID3D12GraphicsCommandList* GetCommandList(void) { return commandList_.Get(); }
    inline size_t GetBackBufferCount() const { return backBuffers_.size(); }

private:
    // 変数

#pragma region DX12変数
    // DebugLayer() で初期化
    ComPtr<ID3D12Debug1> debugController_{ nullptr };

    // DXGIDevice() で初期化
    ComPtr<IDXGIFactory7> dxgiFactory_{ nullptr };
    ComPtr<ID3D12Device> device_{ nullptr };

    // SuppressErrors() で初期化
    ComPtr<ID3D12InfoQueue> infoQueue_{ nullptr };

    // Commands() で初期化
    ComPtr<ID3D12CommandAllocator> cmdAllocator_{ nullptr };
    ComPtr<ID3D12GraphicsCommandList> commandList_{ nullptr };
    ComPtr<ID3D12CommandQueue> commandQueue_{ nullptr };

    // SwapChain() で初期化
    ComPtr<IDXGISwapChain4> swapChain_{ nullptr };

    // RTVDescHeap() で初期化
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc_{};
    ComPtr<ID3D12DescriptorHeap> rtvHeap_{ nullptr };

    // RTV() で初期化
    std::vector<ComPtr<ID3D12Resource>> backBuffers_{}; // バックバッファ

    // DescriptorHeap() で初期化
    std::unique_ptr<DescriptorHeap_t> descHeap_{ nullptr };

    // DepthBuffer() で初期化
    ComPtr<ID3D12Resource> depthBuff_{ nullptr };
    ComPtr<ID3D12DescriptorHeap> dsvHeap_{ nullptr };

    // Fence() で初期化
    ComPtr<ID3D12Fence> fence_{ nullptr };
    UINT64 fenceVal_{};
#pragma endregion

    // Initialize() で使用
    std::unique_ptr<FPSController> fpsController_{ std::make_unique<FPSController>() };

private:
    // 関数
    void DebugLayer(void);
    void DXGIDevice(void);      // アダプタの選別/デバイスの生成 ※Debug only
    void SuppressErrors(void);  // 関数内で指定されたエラーの表示を抑制 ※Debug only
    void Commands(void);        // コマンドアロケータ/リスト/キューの生成
    void SwapChain(WndAPI* p_wndapi);
    void RTVDescHeap(void);     // RTV用デスクリプタヒープの生成 + 生成するための設定（デスクリプタ）
    void RTV(void);             // RTVの生成 + 生成するための設定（デスクリプタ）
    void DescriptorHeap(void);
    void DepthBuffer(void);
    void Fence(void);

    void ClearRTV(void);        // RTVをクリア
    void ClearDepthBuff(void);  // 深度バッファをクリア

    // シングルトン
    InitDirectX(void) {};
    ~InitDirectX(void) {};
    InitDirectX(const InitDirectX&) = delete;
    InitDirectX& operator=(const InitDirectX&) = delete;
};
