#include "ImGuiController.h"
#include "InitDirectX.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

ImGuiController::~ImGuiController(void)
{
    //ImGui_ImplDX12_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();
}

void ImGuiController::Initialize(WndAPI* wndPtr)
{
    //InitDirectX* iDX = InitDirectX::GetInstance();
    wndPtr->kHeight_;
    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();
    //ImGui_ImplWin32_Init(wndPtr->GetHwnd());

    //ImGui_ImplDX12_Init(
    //    iDX->GetDevice(),
    //    static_cast<int32_t>(iDX->GetBackBufferCount()),
    //    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    //    iDX->GetDescHeap_t()->GetDescHeap(),
    //    iDX->GetDescHeap_t()->GetDescHeap()->GetCPUDescriptorHandleForHeapStart(),
    //    iDX->GetDescHeap_t()->GetDescHeap()->GetGPUDescriptorHandleForHeapStart()
    //);

    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
}

void ImGuiController::Begin(void)
{
    //ImGui_ImplWin32_NewFrame();
    //ImGui_ImplDX12_NewFrame();
    //ImGui::NewFrame();
}

void ImGuiController::End(void)
{
    //ImGui::Render();
}

void ImGuiController::Draw(void)
{
    //InitDirectX* iDX = InitDirectX::GetInstance();

    //std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDX->GetDescHeap_t()->GetDescHeap() };
    //iDX->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    //ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), iDX->GetCommandList());
}
