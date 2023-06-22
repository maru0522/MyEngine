#include <d3dx12.h>
#include "WndAPI.h"
#include "PostEffect.h"
#include "InitDirectX.h"

const float  PostEffect::kClearColor[4]{ 0.25f, 0.5f, 0.1f, 0.f }; // ���΂݂�����

PostEffect::PostEffect(void) 
{
    // ���_�o�b�t�@
    std::vector<Sprite::VertexPosUv_t> vertices;
    vertices.emplace_back(Sprite::VertexPosUv_t{{ -1.f, -1.f, 0.0f }, {0.0f, 1.0f} }); // ����
    vertices.emplace_back(Sprite::VertexPosUv_t{{ -1.f, +1.f, 0.0f }, {0.0f, 0.0f} }); // ����
    vertices.emplace_back(Sprite::VertexPosUv_t{{ +1.f, -1.f, 0.0f }, {1.0f, 1.0f} }); // �E��
    vertices.emplace_back(Sprite::VertexPosUv_t{{ +1.f, +1.f, 0.0f }, {1.0f, 0.0f} }); // �E��
    vertexBuffer_.Create(vertices);

    // �萔�o�b�t�@
    cb_.Create();
    cb_.GetConstBuffMap()->matWorld = Math::Matrix::Identity();
    cb_.GetConstBuffMap()->color = { 1.f,1.f,1.f,1.f };
}

void PostEffect::Initialize(void)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // �e�N�X�`���q�[�v�ݒ�
    CD3DX12_HEAP_PROPERTIES textureHeapProp{ D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0 };
    // �e�N�X�`�����\�[�X�ݒ�
    CD3DX12_RESOURCE_DESC textureDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, WndAPI::kWidth_, (UINT)WndAPI::kHeight_, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    // �e�N�X�`���N���A�ݒ�
    CD3DX12_CLEAR_VALUE texClearValue{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kClearColor };
    // �e�N�X�`���o�b�t�@�̐���
    HRESULT hr = iDXPtr->GetDevice()->
        CreateCommittedResource(&textureHeapProp, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &texClearValue, IID_PPV_ARGS(&texBuff_));
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


    // rtv�p�f�X�N�q�[�v�ݒ�
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
    rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDescHeapDesc.NumDescriptors = 1;
    // rtv�p�f�X�N�q�[�v����
    hr = iDXPtr->GetDevice()->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&rtvHeap_));
    assert(SUCCEEDED(hr));

    // rtv�ݒ�
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    // �V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    // �f�X�N�q�[�v��rtv����
    iDXPtr->GetDevice()->CreateRenderTargetView(texBuff_.Get(), &rtvDesc, rtvHeap_->GetCPUDescriptorHandleForHeapStart());


    // �[�x�o�b�t�@�q�[�v�ݒ�
    CD3DX12_HEAP_PROPERTIES depthHeapProp{ D3D12_HEAP_TYPE_DEFAULT };
    // �[�x�o�b�t�@���\�[�X�ݒ�
    CD3DX12_RESOURCE_DESC depthDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WndAPI::kWidth_, WndAPI::kHeight_, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    // �[�x�o�b�t�@�N���A�ݒ�
    CD3DX12_CLEAR_VALUE depthClearValue{ DXGI_FORMAT_D32_FLOAT,1.f,0 };
    // �[�x�o�b�t�@����
    hr = iDXPtr->GetDevice()->CreateCommittedResource(
        &depthHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&depthBuff_));
    assert(SUCCEEDED(hr));


    // dsv�p�f�X�N�q�[�v�ݒ�
    D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc{};
    dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvDescHeapDesc.NumDescriptors = 1;
    // dsv�p�f�X�N�q�[�v����
    hr = iDXPtr->GetDevice()->CreateDescriptorHeap(&dsvDescHeapDesc, IID_PPV_ARGS(&dsvHeap_));
    assert(SUCCEEDED(hr));
    // �f�X�N�q�[�v��dsv����
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    iDXPtr->GetDevice()->CreateDepthStencilView(depthBuff_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::PreDrawScene(void)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // ���\�[�X�o���A�ύX�i�V�F�[�_�[���\�[�X->�`��\�j
    CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    iDXPtr->GetCommandList()->ResourceBarrier(1, &resBarrier);

    // rtv�p�f�X�N�q�[�v�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    // dsv�p�f�X�N�q�[�v�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
    // rtv�Z�b�g
    iDXPtr->GetCommandList()->OMSetRenderTargets(1, &rtvH, false, &dsvH);

    // �r���[�|�[�g�ݒ�
    CD3DX12_VIEWPORT viewPort{ 0.f,0.f,WndAPI::kWidth_,WndAPI::kHeight_ };
    iDXPtr->GetCommandList()->RSSetViewports(1, &viewPort);
    // �V�U�����O��`�ݒ�
    CD3DX12_RECT scissorRect{ 0,0,WndAPI::kWidth_,WndAPI::kHeight_ };
    iDXPtr->GetCommandList()->RSSetScissorRects(1, &scissorRect);

    // �S��ʃN���A
    iDXPtr->GetCommandList()->ClearRenderTargetView(rtvH, kClearColor, 0, nullptr);
    // �[�x�o�b�t�@�N���A
    iDXPtr->GetCommandList()->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
}

void PostEffect::Draw(void)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // �v���~�e�B�u�`��̐ݒ�R�}���h
    iDXPtr->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`���X�g

    // SRV�q�[�v�̐ݒ�R�}���h
    //std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDXPtr->GetDescHeap_t()->GetDescHeap() };
    std::vector<ID3D12DescriptorHeap*> ppHeaps = { srvHeap_.Get() };
    iDXPtr->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
    //ID3D12PipelineState* plsPtr{ GraphicsPipeline::GetInstance()->GetPipeline2d(GraphicsPipeline::BlendMode::NONE).pipelineState.Get() };
    iDXPtr->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSO("PSO_POSTEFFECT", GraphicsPipeline::BlendMode::NONE)->pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("PSO_POSTEFFECT", GraphicsPipeline::BlendMode::NONE)->rootSignature.Get());

    // ���_�o�b�t�@�r���[�̐ݒ�R�}���h
    iDXPtr->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVbView());

    // �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());

    // SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^0�Ԃɐݒ�
    //iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, imagePtr_->srvGpuHandle);
    iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHeap_->GetGPUDescriptorHandleForHeapStart());

    // �`��R�}���h
    iDXPtr->GetCommandList()->DrawInstanced((unsigned int)vertexBuffer_.GetVerticesNum(), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void PostEffect::PostDrawScene(void)
{
    // ���\�[�X�o���A�ύX�i�`��\->�V�F�[�_�[���\�[�X�j
    CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    InitDirectX::GetInstance()->GetCommandList()->ResourceBarrier(1, &resBarrier);
}
