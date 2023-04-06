#include "Sprite.h"
#include "InitDirectX.h"
#include "GraphicsPipeline.h"

using BlendMode = HelperGraphicPipeline::BlendMode;

void Sprite::PreDraw(void)
{
    InitDirectX* iDX = InitDirectX::GetInstance();

# pragma region 共通
    // パイプラインステートとルートシグネチャの設定コマンド
    iDX->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline2d(BlendMode::NONE).pipelineState_.Get());
    iDX->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline2d(BlendMode::NONE).rootSignature_.Get());
    // プリミティブ形状の設定コマンド
    iDX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト

    ID3D12DescriptorHeap* ppHeaps[] = { iDX->GetDescHeap_t()->GetDescHeap() };
    // SRVヒープの設定コマンド
    iDX->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
#pragma endregion
}
