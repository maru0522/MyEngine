#include "ImGuiController.h"
#include "InitDirectX.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

ImGuiController::~ImGuiController(void)
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiController::Initialize(WndAPI* wndPtr)
{
    InitDirectX* iDX = InitDirectX::GetInstance();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(wndPtr->GetHwnd());

    CreateSRVHeap();
    ImGui_ImplDX12_Init(
        iDX->GetDevice(),
        static_cast<int32_t>(iDX->GetBackBufferCount()),
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        srvHeap_.Get(),
        srvHeap_->GetCPUDescriptorHandleForHeapStart(),
        srvHeap_->GetGPUDescriptorHandleForHeapStart()
    );

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
}

void ImGuiController::Begin(void)
{
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
}

void ImGuiController::End(void)
{
    ImGui::Render();
}

void ImGuiController::Draw(void)
{
    InitDirectX* iDX = InitDirectX::GetInstance();

    ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_.Get() };
    iDX->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), iDX->GetCommandList());
}

void ImGuiController::CreateSRVHeap(void)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = sMAX_SRV_COUNT_FOR_IMGUI;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
    assert(SUCCEEDED(hr));
}
