#include "Object3D.h"
#include "InitDirectX.h"

LightGroup* Object3D::sLightGroupPtr_{ nullptr };
ModelManager* Object3D::sModelMPtr_{ nullptr };
TextureManager* Object3D::sTexMPtr_{ nullptr };
ConstBuffer<Object3D::CBMatViewPerse_t> Object3D::sCbMatViewPerse_{};

void Object3D::StaticInitialize(ModelManager* modelMPtr, TextureManager* texMPtr)
{
    sModelMPtr_ = modelMPtr;
    sTexMPtr_ = texMPtr;
    sCbMatViewPerse_.Create();

    // ���������̓J���������݂��Ȃ�����CameraManager��SetCurrentCamera()���Ɏ��s����悤�ɂ���
    //UpdateCBMatViewPerse();
}

void Object3D::UpdateCBMatViewPerse(CameraManager* camMPtr)
{
    sCbMatViewPerse_.GetConstBuffMap()->matView = camMPtr->GetCurrentCamera()->GetMatView();
    sCbMatViewPerse_.GetConstBuffMap()->matPerspective = camMPtr->GetCurrentCamera()->GetMatProjPerspective();

    sCbMatViewPerse_.GetConstBuffMap()->cameraPos = camMPtr->GetCurrentCamera()->GetCoordinatePtr()->GetPosition();
}

void Object3D::PreDraw(BlendMode blendmode)
{
    // �C���X�^���X�擾
    InitDirectX* iDX = InitDirectX::GetInstance();

    // �v���~�e�B�u�`��̐ݒ�R�}���h
    iDX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

    std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDX->GetDescHeap_t()->GetDescHeap() };
    // SRV�q�[�v�̐ݒ�R�}���h
    iDX->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
    iDX->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D",blendmode)->pipelineState.Get());
    iDX->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D", blendmode)->rootSignature.Get());
}

void Object3D::SetDrawBlendMode(BlendMode blendmode)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
    iDXPtr->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D", blendmode)->pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D", blendmode)->rootSignature.Get());
}

Object3D::Object3D(const fsPath& path) :
    parent_(nullptr)
{
    // ���f���ǂݍ���
    model_ = sModelMPtr_->GetModel(path);
    // ���f���̃}�e���A���p�萔�o�b�t�@�𐶐�
    model_.cbMaterial_.Create();
    // �}�e���A����萔�o�b�t�@�֓]��
    model_.UpdateCB();

    // �萔�o�b�t�@����
    cb_.Create();

    transform_.Initialize();
    axes_.Initialize();
}

void Object3D::Update(void)
{
    // ���W�v�Z�Ɠ]��
    coordinate_.mat_world
    cb_.GetConstBuffMap()->matWorld = coordinate_.mat_world;
    model_.UpdateCB();
}

void Object3D::Draw(void)
{
    Draw(sTexMPtr_->GetImagePtr(model_.material_.texKey)->srvGpuHandle);
}

void Object3D::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE& texture)
{
    if (isInvisible_) return;

    InitDirectX* iDX = InitDirectX::GetInstance();

    // ���_�o�b�t�@
    iDX->GetCommandList()->IASetVertexBuffers(0, 1, &model_.meshPtr_->GetVBPtr()->GetVbView());
    // �C���f�b�N�X�o�b�t�@
    iDX->GetCommandList()->IASetIndexBuffer(&model_.meshPtr_->GetIBPtr()->GetIbView());

    // �萔�o�b�t�@�r���[�iCBV�j�̐ݒ�R�}���h
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(2, sCbMatViewPerse_.GetBuffer()->GetGPUVirtualAddress());
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(3, model_.cbMaterial_.GetBuffer()->GetGPUVirtualAddress());

    //�w��̃q�[�v�ɂ���SRV�����[�g�p�����[�^0�Ԃɐݒ�
    iDX->GetCommandList()->SetGraphicsRootDescriptorTable(0, texture);

    // �`��R�}���h���X�g
    iDX->GetCommandList()->DrawIndexedInstanced((uint32_t)model_.meshPtr_->GetIBPtr()->GetIndicesNum(), 1, 0, 0, 0);
}

void Object3D::Draw(const fsPath& path)
{
    Draw(sTexMPtr_->GetImagePtr(path)->srvGpuHandle);
}