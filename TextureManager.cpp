#include "TextureManager.h"
#include "Util.h"
#include "Vector4.h"

void TextureManager::Initialize(InitDirectX* p_idx)
{
    p_idx_ = p_idx;

    GenerateMissingImage();
}

void TextureManager::Register(const Image& img)
{
    Register(img, "none", false);
}

void TextureManager::Register(const Image& img, const std::string& nickname, bool isAddNickname = true)
{
    if (isAddNickname) {
        // nickname���o�^�ς݂��`�F�b�N
        if (paths_.count(nickname)) {
            // �o�^�ς݂̏ꍇ���O�o��
            Util::Log::PrintOutputWindow("[TextureManager] : This nickname is already in use. (" + nickname + ") The registration has therefore been cancelled.");

            // �o�^�͍s��Ȃ��B
            return;
        }

        // nickname�̗v�f�\�z
        paths_.emplace(nickname, img.path_);
    }

    // Image���o�^�ς݂��`�F�b�N
    if (textures_.count(img.path_)) {
        // �o�^�ς݂̏ꍇ���O�o��
        Util::Log::PrintOutputWindow("[TextureManager] : This image is already loaded. (" + img.path_.string() + ")");

        // �o�^�͍s��Ȃ��B
        return;
    }

    // �v�f�\�z
    textures_.emplace(img.path_, img);
}

void TextureManager::GenerateMissingImage(void)
{
#pragma region missingTexture����
    Image tempImg{}; // �Ă�ۂ���

    tempImg.path_ = "MISSING";

    // ��ӂ̃s�N�Z����
    constexpr size_t imageLength{ 256 };
    // �z��̗v�f��
    constexpr size_t imageDataCount{ imageLength * imageLength };

    // �C���[�W�f�[�^�z��
    Vector4* imageData{ new Vector4[imageDataCount] };

    // ����
    for (size_t i = 0; i < imageDataCount; i++) {
        if (i < 32513) {
            if (i % 256 < 128) {
                imageData[i] = { 0.0f, 0.0f, 0.0f, 1.0f };
            }
            else if (i % 256 >= 128) {
                imageData[i] = { 1.0f, 0.0f, 1.0f, 1.0f };
            }
        }
        else {
            if (i % 256 < 128) {
                imageData[i] = { 1.0f, 0.0f, 1.0f, 1.0f };
            }
            else if (i % 256 >= 128) {
                imageData[i] = { 0.0f, 0.0f, 0.0f, 1.0f };
            }
        }
    }

#pragma endregion

#pragma region �q�[�v�ݒ�ƃf�X�N���v�^�ݒ�
    // �q�[�v�ݒ�
    D3D12_HEAP_PROPERTIES texHeapProp{};
    texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
    texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    // ���\�[�X�ݒ�
    D3D12_RESOURCE_DESC textureResourceDesc{};
    textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureResourceDesc.Width = imageLength;
    textureResourceDesc.Height = imageLength;
    textureResourceDesc.DepthOrArraySize = 1;
    textureResourceDesc.MipLevels = 1;
    textureResourceDesc.SampleDesc.Count = 1;
#pragma endregion

#pragma region �e�N�X�`���o�b�t�@
    // �e�N�X�`���o�b�t�@�̐���
    HRESULT r = p_idx_->GetDevice()->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&tempImg.buff_));

    assert(SUCCEEDED(r));
#pragma endregion

#pragma region �o�b�t�@�ւ̃f�[�^�]��
    // �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
    r = tempImg.buff_->WriteToSubresource(
            0,
            nullptr,		// �S�̈�փR�s�[
            imageData,	// ���f�[�^�A�h���X
            sizeof(DirectX::XMFLOAT4) * imageLength,    // 1���C���T�C�Y
            sizeof(DirectX::XMFLOAT4) * imageDataCount  // �S�T�C�Y
        );

    //�C���[�W�f�[�^���
    delete[] imageData;

    assert(SUCCEEDED(r));
#pragma endregion

    // InitDirectX����DescriptorHeap�ɐ����B
    // �Ԃ�l��gpu�̃A�h���X���擾�B
    tempImg.srvGpuHandle_.ptr = p_idx_->GetDescHeap()->CreateSRV(textureResourceDesc, tempImg.buff_.Get());

    // �o�^
    Register(tempImg);
}
