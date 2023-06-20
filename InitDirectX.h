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
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct DescriptorHeap_t
    {
    public:
        // �֐�
        DescriptorHeap_t(InitDirectX* p_idx);

        // srv���q�[�v�ɐ���
        size_t CreateSRV(const D3D12_RESOURCE_DESC& rscDesc, ID3D12Resource* p_rsc);

        // setter
        inline void SetMaxSrvDesc(uint32_t maxSrv) { maxSRVDesc = maxSrv; }

        // getter
        inline ID3D12DescriptorHeap* GetDescHeap(void) { return descriptorHeap_.Get(); }

    private:
        // �ϐ�
        // DescriptorHeap�R���X�g���N�^ �ŏ�����
        ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
        uint32_t maxSRVDesc{ 256 };
        uint32_t maxCBVDesc{ 256 };
        uint32_t maxUAVDesc{ 256 };
        uint32_t SRVCount{ 1 }; // ImGui�p�̃f�X�N���v�^�����̂���1��������B
        uint32_t CBVCount{};
        uint32_t UAVCount{};

        InitDirectX* iDXPtr{ nullptr };
    };

public:
    // �ÓI�֐�
    static InitDirectX* GetInstance(void);

    // �֐�
    void Initialize(WndAPI* p_wndapi);
    void PreDraw(void);
    void PostDraw(void);

    // getter
    inline ID3D12Device* GetDevice(void) { return device_.Get(); }
    inline DescriptorHeap_t* GetDescHeap_t(void) { return descHeap_.get(); }
    inline ID3D12GraphicsCommandList* GetCommandList(void) { return commandList_.Get(); }
    inline size_t GetBackBufferCount() const { return backBuffers_.size(); }

private:
    // �ϐ�

#pragma region DX12�ϐ�
    // DebugLayer() �ŏ�����
    ComPtr<ID3D12Debug1> debugController_{ nullptr };

    // DXGIDevice() �ŏ�����
    ComPtr<IDXGIFactory7> dxgiFactory_{ nullptr };
    ComPtr<ID3D12Device> device_{ nullptr };

    // SuppressErrors() �ŏ�����
    ComPtr<ID3D12InfoQueue> infoQueue_{ nullptr };

    // Commands() �ŏ�����
    ComPtr<ID3D12CommandAllocator> cmdAllocator_{ nullptr };
    ComPtr<ID3D12GraphicsCommandList> commandList_{ nullptr };
    ComPtr<ID3D12CommandQueue> commandQueue_{ nullptr };

    // SwapChain() �ŏ�����
    ComPtr<IDXGISwapChain4> swapChain_{ nullptr };

    // RTVDescHeap() �ŏ�����
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc_{};
    ComPtr<ID3D12DescriptorHeap> rtvHeap_{ nullptr };

    // RTV() �ŏ�����
    std::vector<ComPtr<ID3D12Resource>> backBuffers_{}; // �o�b�N�o�b�t�@

    // DescriptorHeap() �ŏ�����
    std::unique_ptr<DescriptorHeap_t> descHeap_{ nullptr };

    // DepthBuffer() �ŏ�����
    ComPtr<ID3D12Resource> depthBuff_{ nullptr };
    ComPtr<ID3D12DescriptorHeap> dsvHeap_{ nullptr };

    // Fence() �ŏ�����
    ComPtr<ID3D12Fence> fence_{ nullptr };
    UINT64 fenceVal_{};
#pragma endregion

    // Initialize() �Ŏg�p
    std::unique_ptr<FPSController> fpsController_{ std::make_unique<FPSController>() };

private:
    // �֐�
    void DebugLayer(void);
    void DXGIDevice(void);      // �A�_�v�^�̑I��/�f�o�C�X�̐��� ��Debug only
    void SuppressErrors(void);  // �֐����Ŏw�肳�ꂽ�G���[�̕\����}�� ��Debug only
    void Commands(void);        // �R�}���h�A���P�[�^/���X�g/�L���[�̐���
    void SwapChain(WndAPI* p_wndapi);
    void RTVDescHeap(void);     // RTV�p�f�X�N���v�^�q�[�v�̐��� + �������邽�߂̐ݒ�i�f�X�N���v�^�j
    void RTV(void);             // RTV�̐��� + �������邽�߂̐ݒ�i�f�X�N���v�^�j
    void DescriptorHeap(void);
    void DepthBuffer(void);
    void Fence(void);

    void ClearRTV(void);        // RTV���N���A
    void ClearDepthBuff(void);  // �[�x�o�b�t�@���N���A

    // �V���O���g��
    InitDirectX(void) {};
    ~InitDirectX(void) {};
    InitDirectX(const InitDirectX&) = delete;
    InitDirectX& operator=(const InitDirectX&) = delete;
};
