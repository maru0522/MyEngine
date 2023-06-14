#pragma once
#include "WndAPI.h"
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class ImGuiController
{
    public:
        // 定義
        static constexpr uint32_t kMAX_SRV_COUNT_FOR_IMGUI{ 1 };

        // 関数
        ImGuiController(void) {};
        ~ImGuiController(void);

        void Initialize(WndAPI* wndPtr);
        void Begin(void);
        void End(void);
        void Draw(void);

    private:
        void CreateSRVHeap(void);

    private:
        // 変数
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

