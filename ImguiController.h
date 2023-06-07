#pragma once
#include "WndAPI.h"
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class ImGuiController
{
    public:
        // ��`
        static constexpr uint32_t sMAX_SRV_COUNT_FOR_IMGUI{ 1 };

        // �֐�
        ImGuiController(void) {};
        ~ImGuiController(void);

        void Initialize(WndAPI* wndPtr);
        void Begin(void);
        void End(void);
        void Draw(void);

    private:
        void CreateSRVHeap(void);

    private:
        // �ϐ�
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

