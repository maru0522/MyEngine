#include "PostEffect.h"
#include "InitDirectX.h"

PostEffect::PostEffect(void) : Sprite("")
{
    this->SetSize({ 500.f,500.f });
}

void PostEffect::Draw(void)
{
    TransferVertex();
    UpdateMatrix();

    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // プリミティブ形状の設定コマンド
    iDXPtr->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト

    // SRVヒープの設定コマンド
    std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDXPtr->GetDescHeap_t()->GetDescHeap() };
    iDXPtr->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // パイプラインステートとルートシグネチャの設定コマンド
    iDXPtr->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).rootSignature.Get());

    // 頂点バッファビューの設定コマンド
    iDXPtr->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVbView());

    // 定数バッファビュー(CBV)の設定コマンド
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(2, sCbMatOrthoGraphic_.GetBuffer()->GetGPUVirtualAddress());

    // SRVヒープの先頭にあるSRVをルートパラメータ0番に設定
    iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, imagePtr_->srvGpuHandle);

    // 描画コマンド
    iDXPtr->GetCommandList()->DrawInstanced((unsigned int)vertexBuffer_.GetVerticesNum(), 1, 0, 0); // 全ての頂点を使って描画
}
