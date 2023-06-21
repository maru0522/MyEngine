#include <d3dx12.h>
#include "WndAPI.h"
#include "PostEffect.h"
#include "InitDirectX.h"

PostEffect::PostEffect(void) : Sprite("")
{
    this->SetSize({ 500.f,500.f });
}

void PostEffect::Initialize(void)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // �e�N�X�`���q�[�v�ݒ�
    CD3DX12_HEAP_PROPERTIES textureHeapProp{ D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0 };

    // �e�N�X�`�����\�[�X�ݒ�
    CD3DX12_RESOURCE_DESC textureDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, WndAPI::kWidth_, (UINT)WndAPI::kHeight_, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    // �e�N�X�`���o�b�t�@�̐���
    HRESULT hr = iDXPtr->GetDevice()->CreateCommittedResource(
        &textureHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&texBuff_));
    assert(SUCCEEDED(hr));

    // �e�N�X�`����ԃN���A
    constexpr uint32_t pixelCount = WndAPI::kWidth_ * WndAPI::kHeight_; // ��f��
    constexpr uint32_t rowPitch = sizeof(uint32_t) * WndAPI::kWidth_;   // 1�s���̃f�[�^�T�C�Y
    constexpr uint32_t deothPitch = rowPitch * WndAPI::kHeight_;        // �S�̂̃f�[�^�T�C�Y
    // �C���[�W
    std::vector<uint32_t> img(pixelCount);
    for (size_t i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

    // �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
    hr = texBuff_->WriteToSubresource(0, nullptr, img.data(), rowPitch, deothPitch);
    assert(SUCCEEDED(hr));


    // srv�pdescHeap�ݒ�
    D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc{};
    srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvDescHeapDesc.NumDescriptors = 1;
    // srv�pdescHeap����
    hr = iDXPtr->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&srvHeap_));
    assert(SUCCEEDED(hr));

    // srv�ݒ�
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    // descHeap��srv����
    iDXPtr->GetDevice()->CreateShaderResourceView(texBuff_.Get(), &srvDesc, srvHeap_->GetCPUDescriptorHandleForHeapStart());
    //iDXPtr->GetDescHeap_t()->CreateSRV(textureDesc, texBuff_.Get());
}

void PostEffect::Draw(void)
{
    TransferVertex();
    UpdateMatrix();

    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // �v���~�e�B�u�`��̐ݒ�R�}���h
    iDXPtr->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`���X�g

    // SRV�q�[�v�̐ݒ�R�}���h
    //std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDXPtr->GetDescHeap_t()->GetDescHeap() };
    std::vector<ID3D12DescriptorHeap*> ppHeaps = { srvHeap_.Get() };
    iDXPtr->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
    //ID3D12PipelineState* plsPtr{ GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).pipelineState.Get() };
    iDXPtr->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).rootSignature.Get());

    // ���_�o�b�t�@�r���[�̐ݒ�R�}���h
    iDXPtr->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVbView());

    // �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(2, sCbMatOrthoGraphic_.GetBuffer()->GetGPUVirtualAddress());

    // SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^0�Ԃɐݒ�
    //iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, imagePtr_->srvGpuHandle);
    iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHeap_->GetGPUDescriptorHandleForHeapStart());

    // �`��R�}���h
    iDXPtr->GetCommandList()->DrawInstanced((unsigned int)vertexBuffer_.GetVerticesNum(), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}
