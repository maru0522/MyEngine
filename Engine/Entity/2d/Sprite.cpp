#include <vector>
#include "Sprite.h"
#include "Matrix4.h"
#include "MathUtil.h"
#include "InitDirectX.h"
#include "CameraManager.h"
#include "TextureManager.h"
#include "PSOManager.h"

TextureManager* Sprite::sTexMPtr_{ nullptr };
ConstBuffer<Sprite::CBMatOrthoGraphic_t> Sprite::sCbMatOrthoGraphic_{};

void Sprite::StaticInitialize(TextureManager* texMPtr)
{
    sTexMPtr_ = texMPtr;
    sCbMatOrthoGraphic_.Create();

    // ���������̓J���������݂��Ȃ�����CameraManager��SetCurrentCamera()���Ɏ��s����悤�ɂ���
    //UpdateCBMatOrthoGraphic();
}

void Sprite::PreDraw(BlendMode blendmode)
{
    InitDirectX* iDX = InitDirectX::GetInstance();

    // �v���~�e�B�u�`��̐ݒ�R�}���h
    iDX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // �O�p�`���X�g

    // SRV�q�[�v�̐ݒ�R�}���h
    std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDX->GetDescHeap_t()->GetDescHeap() };
    iDX->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
    iDX->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSOPtr("PSO_SPRITE", blendmode)->pipelineState.Get());
    iDX->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSOPtr("PSO_SPRITE", blendmode)->rootSignature.Get());
}

void Sprite::SetDrawBlendMode(BlendMode blendmode)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // �p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
    iDXPtr->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSOPtr("PSO_SPRITE", blendmode)->pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSOPtr("PSO_SPRITE", blendmode)->rootSignature.Get());
}

void Sprite::UpdateCBMatOrthoGraphic(CameraManager* camMPtr)
{
    // ���s���e�s��ɃJ�����̕ێ����镽�s���e�s�����
    sCbMatOrthoGraphic_.GetConstBuffMap()->matOrthoGraphic = camMPtr->GetCurrentCamera()->GetMatProjOrthoGraphic();
}

Sprite::Sprite(const fsPath& path, const std::string& nickname) :
    parent_(nullptr),
    position_(0.f, 0.f), scale_(1.f, 1.f), size_(0.f, 0.f), rotation_(0.f),
    imagePtr_(nullptr)
{
    nickname == "noAssign" ?
        imagePtr_ = sTexMPtr_->GetImagePtr(path) :
        imagePtr_ = sTexMPtr_->GetImagePtrByNickname(nickname);

    size_.x = (float)imagePtr_->buff->GetDesc().Width;
    size_.y = (float)imagePtr_->buff->GetDesc().Height;

    cutLength_.x = (float)imagePtr_->buff->GetDesc().Width;
    cutLength_.y = (float)imagePtr_->buff->GetDesc().Height;

    // �萔�o�b�t�@����
    cb_.Create();

    std::vector<VertexPosUv_t> vertices;
    vertices.emplace_back(VertexPosUv_t{ {   0.0f, 100.0f, 0.0f }, {0.0f, 1.0f} }); // ����
    vertices.emplace_back(VertexPosUv_t{ {   0.0f,   0.0f, 0.0f }, {0.0f, 0.0f} }); // ����
    vertices.emplace_back(VertexPosUv_t{ { 100.0f, 100.0f, 0.0f }, {1.0f, 1.0f} }); // �E��
    vertices.emplace_back(VertexPosUv_t{ { 100.0f,   0.0f, 0.0f }, {1.0f, 0.0f} }); // �E��

    vertexBuffer_.Create(vertices);

    SetColor({ 1.f, 1.f, 1.f, 1.f });
}

void Sprite::Update(void)
{
    TransferVertex();
    UpdateMatrix();
}

void Sprite::Draw(void)
{
    if (isInvisible_) return;

    InitDirectX* iDXPtr = InitDirectX::GetInstance();

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

void Sprite::TransferVertex(void)
{
    // �A���J�[�|�C���g
    float left{ (0.f - anchorPoint_.x) * size_.x * scale_.x };
    float right{ (1.f - anchorPoint_.x) * size_.x * scale_.x };
    float top{ (0.f - anchorPoint_.y) * size_.y * scale_.y };
    float bottom{ (1.f - anchorPoint_.y) * size_.y * scale_.y };

    // ���]
    if (isFlipX_) {
        left = -left;
        right = -right;
    }

    if (isFlipY_) {
        top = -top;
        bottom = -bottom;
    }

    // ���_���W
    std::vector<VertexPosUv_t> vertices;
    vertices.emplace_back(VertexPosUv_t{ {   left, bottom, 0.0f }, {0.0f, 1.0f} }); // ����
    vertices.emplace_back(VertexPosUv_t{ {   left,    top, 0.0f }, {0.0f, 0.0f} }); // ����
    vertices.emplace_back(VertexPosUv_t{ {  right, bottom, 0.0f }, {1.0f, 1.0f} }); // �E��
    vertices.emplace_back(VertexPosUv_t{ {  right,    top, 0.0f }, {1.0f, 0.0f} }); // �E��

    // uv���W
    float texLeft{ cutStartPoint_.x / imagePtr_->buff->GetDesc().Width };
    float texRight{ (cutStartPoint_.x + cutLength_.x) / imagePtr_->buff->GetDesc().Width };
    float texTop{ cutStartPoint_.y / imagePtr_->buff->GetDesc().Height };
    float texBottom{ (cutStartPoint_.y + cutLength_.y) / imagePtr_->buff->GetDesc().Height };

    vertices[0].uv = { texLeft, texBottom };
    vertices[1].uv = { texLeft,    texTop };
    vertices[2].uv = { texRight, texBottom };
    vertices[3].uv = { texRight,    texTop };

    vertexBuffer_.TransferVertexToBuffer(vertices);
}

void Sprite::UpdateMatrix(void)
{
    matWorld_ = Math::Mat4::Identity();
    Matrix4 matRotation{ Math::Mat4::Identity() };

    matRotation *= Math::Mat4::RotationZ(rotation_);
    matWorld_ *= matRotation;

    matWorld_ = Math::Mat4::Translate(matWorld_, { position_.x,position_.y,0.f });

    if (parent_) matWorld_ *= parent_->matWorld_;

    // �萔�o�b�t�@�ɓ]��
    cb_.GetConstBuffMap()->matWorld = matWorld_;
}

void Sprite::SetColor(const Vector4& rgba)
{
    Vector4 lrgba = rgba;

    // �l��0.f~1.f���Ɏ��߂�
    lrgba.x = Math::Function::Clamp(rgba.x, 0.f, 1.f);
    lrgba.y = Math::Function::Clamp(rgba.y, 0.f, 1.f);
    lrgba.z = Math::Function::Clamp(rgba.z, 0.f, 1.f);
    lrgba.w = Math::Function::Clamp(rgba.w, 0.f, 1.f);

    // �l���������ނƎ����I�ɓ]�������
    cb_.GetConstBuffMap()->color = lrgba;
}

void Sprite::SetColor255(const Vector4& rgba)
{
    Vector4 lrgba = rgba;

    // �l��0.f~255.f���Ɏ��߂�
    lrgba.x = Math::Function::Clamp(rgba.x, 0.f, 255.f);
    lrgba.y = Math::Function::Clamp(rgba.y, 0.f, 255.f);
    lrgba.z = Math::Function::Clamp(rgba.z, 0.f, 255.f);
    lrgba.w = Math::Function::Clamp(rgba.w, 0.f, 255.f);

    // ���K��
    lrgba = { lrgba.x / 255.f, lrgba.y / 255.f, lrgba.z / 255.f, lrgba.w / 255.f };

    // �l���������ނƎ����I�ɓ]�������
    cb_.GetConstBuffMap()->color = lrgba;
}

