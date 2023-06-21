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

    // �v���~�e�B�u�`��̐ݒ�R�}���h
    iDXPtr->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`���X�g

    // SRV�q�[�v�̐ݒ�R�}���h
    std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDXPtr->GetDescHeap_t()->GetDescHeap() };
    iDXPtr->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
    iDXPtr->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).rootSignature.Get());

    // ���_�o�b�t�@�r���[�̐ݒ�R�}���h
    iDXPtr->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVbView());

    // �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(2, sCbMatOrthoGraphic_.GetBuffer()->GetGPUVirtualAddress());

    // SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^0�Ԃɐݒ�
    iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, imagePtr_->srvGpuHandle);

    // �`��R�}���h
    iDXPtr->GetCommandList()->DrawInstanced((unsigned int)vertexBuffer_.GetVerticesNum(), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}
