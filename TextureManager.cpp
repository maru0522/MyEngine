#include "TextureManager.h"
#include "Util.h"
#include "Vector4.h"
#include <DirectXTex.h>
#include <cassert>

void TextureManager::Initialize(void)
{
    GenerateMissingImage();
}

void TextureManager::Load(const fsPath& path)
{
    InitDirectX* p_idx{ InitDirectX::GetInstance() };

    Image tempImg{}; // �Ă�ۂ���

    tempImg.path_ = path;

    // Image�d���m�F
    if (CheckDuplicateTexture(tempImg)) return;

#pragma region �e�N�X�`���ǂݍ���
    //�摜�C���[�W�f�[�^�z��
    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};

    // WIC�e�N�X�`���̃��[�h�Ɏg�� path�𕶎���ϊ�
    std::string strPath{ tempImg.path_.string() };
    std::wstring wStrPath{ strPath.begin(), strPath.end() };
    const wchar_t* szFile{ wStrPath.c_str() };

    // WIC�e�N�X�`���̃��[�h
    HRESULT hr = LoadFromWICFile(szFile, DirectX::WIC_FLAGS_NONE, &metadata, scratchImg);
    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region �~�b�v�}�b�v
    DirectX::ScratchImage mipChain{};

    // �~�b�v�}�b�v����
    hr = GenerateMipMaps(scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
    if (SUCCEEDED(hr)) {
        scratchImg = std::move(mipChain);
        metadata = scratchImg.GetMetadata();
    }
#pragma endregion

    // �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
    metadata.format = DirectX::MakeSRGB(metadata.format);

#pragma region �q�[�v�ݒ�ƃf�X�N���v�^�ݒ�
    // �q�[�v�ݒ�
    D3D12_HEAP_PROPERTIES texHeapProp{};
    texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
    texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    // ���\�[�X�ݒ�
    D3D12_RESOURCE_DESC textureResourceDesc{};
    textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDesc.Format = metadata.format;
    textureResourceDesc.Width = metadata.width;
    textureResourceDesc.Height = static_cast<UINT>(metadata.height);
    textureResourceDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
    textureResourceDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
    textureResourceDesc.SampleDesc.Count = 1;
#pragma endregion

#pragma region �e�N�X�`���o�b�t�@
    // �e�N�X�`���o�b�t�@�̐���
    hr = p_idx->GetDevice()->CreateCommittedResource(&texHeapProp, D3D12_HEAP_FLAG_NONE, &textureResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&tempImg.buff_));
    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region �o�b�t�@�ւ̃f�[�^�]��
    // �S�~�b�v�}�b�v�ɂ���
    for (size_t i = 0; i < metadata.mipLevels; i++) {
        // �~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
        const DirectX::Image* img = scratchImg.GetImage(i, 0, 0);

        // �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
        hr = tempImg.buff_->WriteToSubresource(
            static_cast<UINT>(i),
            nullptr,		// �S�̈�փR�s�[
            img->pixels,	// ���f�[�^�A�h���X
            static_cast<UINT>(img->rowPitch),	// 1���C���T�C�Y
            static_cast<UINT>(img->slicePitch)	// �S�T�C�Y
        );
        assert(SUCCEEDED(hr));
    }
#pragma endregion

    // InitDirectX����DescriptorHeap�ɐ����B
    // �Ԃ�l��gpu�̃A�h���X���擾�B
    tempImg.srvGpuHandle_.ptr = p_idx->GetDescHeap_t()->CreateSRV(textureResourceDesc, tempImg.buff_.Get());

    // �o�^
    RegisterImg(tempImg);
}

void TextureManager::Load(const fsPath& path, const std::string& nickname)
{
    // nickname�̏d���m�F
    if (CheckDuplicateNickname(nickname)) return;

    Load(path);

    //nickname�o�^
    RegisterNickname(path, nickname);
}

void TextureManager::LoadFolder(const fsPath& path)
{
    for (const std::experimental::filesystem::directory_entry& file : std::experimental::filesystem::directory_iterator(path)) {
        std::experimental::filesystem::path fileName{ file.path().filename().string() };

        if (file.path().extension() == ".png" || file.path().extension() == ".jpg" || file.path().extension() == ".jpeg") {
            Load(path / fileName);
        }
    }
}

const TextureManager::Image* TextureManager::GetImagePtr(const fsPath& path) const
{
    try
    {
        return &textures_.at(path);
    }
    catch (const std::out_of_range&)
    {
        // �G���[���O�o��
        Util::Log::PrintOutputWindow("[TextureManager] : Couldn't find a texture corresponding to the argument path(" + path.string() + "), so replaced it with \"Missing Texture\". ");
        return &textures_.at("MISSING");
    }
}

const TextureManager::Image* TextureManager::GetImagePtrByNickname(const std::string& nickname)
{
    fsPath path{};

    try
    {
        path = paths_.at(nickname);
    }
    catch (const std::exception&)
    {
        // �G���[���O�o��
        Util::Log::PrintOutputWindow("[TextureManager] : Couldn't find a path corresponding to the argument nickname(" + nickname + "), so replaced it with \"MISSING\". ");
        path = "MISSING";
    }

    return GetImagePtr(path);
}

void TextureManager::RegisterImg(const Image& img)
{
    // Image�̗v�f�\�z
    textures_.emplace(img.path_, img);
}

void TextureManager::RegisterNickname(const fsPath& path, const std::string& nickname)
{
    // nickname�̗v�f�\�z
    paths_.emplace(nickname, path);
}

void TextureManager::GenerateMissingImage(void)
{
    InitDirectX* p_idx{ InitDirectX::GetInstance() };

#pragma region missingTexture����
    Image tempImg{}; // �Ă�ۂ���

    tempImg.path_ = "MISSING";

    // ��ӂ̃s�N�Z����
    constexpr size_t imageLength{ 256 };
    // �z��̗v�f��
    constexpr size_t imageDataCount{ imageLength * imageLength };

    // �C���[�W�f�[�^�z��
    std::vector<Vector4> imageData;

    // ����
    for (size_t i = 0; i < imageDataCount; i++) {
        if (i < 32513) {
            if (i % 256 < 128) {
                imageData.emplace_back(Vector4{ 0.0f, 0.0f, 0.0f, 1.0f });
            }
            else if (i % 256 >= 128) {
                imageData.emplace_back(Vector4{ 0.5f, 0.5f, 0.5f, 1.0f });
            }
        }
        else {
            if (i % 256 < 128) {
                imageData.emplace_back(Vector4{ 0.5f, 0.5f, 0.5f, 1.0f });
            }
            else if (i % 256 >= 128) {
                imageData.emplace_back(Vector4{ 0.0f, 0.0f, 0.0f, 1.0f });
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
    HRESULT hr = p_idx->GetDevice()->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&tempImg.buff_));

    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region �o�b�t�@�ւ̃f�[�^�]��
    // �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
    hr = tempImg.buff_->WriteToSubresource(
        0,
        nullptr,		// �S�̈�փR�s�[
        imageData.data(),	// ���f�[�^�A�h���X
        sizeof(DirectX::XMFLOAT4) * imageLength,    // 1���C���T�C�Y
        sizeof(DirectX::XMFLOAT4) * imageDataCount  // �S�T�C�Y
    );

    assert(SUCCEEDED(hr));
#pragma endregion

    // InitDirectX����DescriptorHeap�ɐ����B
    // �Ԃ�l��gpu�̃A�h���X���擾�B
    tempImg.srvGpuHandle_.ptr = p_idx->GetDescHeap_t()->CreateSRV(textureResourceDesc, tempImg.buff_.Get());

    // �o�^
    RegisterImg(tempImg);
}

bool TextureManager::CheckDuplicateTexture(const Image& img)
{
    // Image���o�^�ς݂��`�F�b�N
    if (textures_.count(img.path_)) {
        // �o�^�ς݂̏ꍇ���O�o��
        Util::Log::PrintOutputWindow("[TextureManager] : This image is already loaded. (" + img.path_.string() + ")");

        return true;
    }
    return false;
}

bool TextureManager::CheckDuplicateNickname(const std::string& nickname)
{
    // nickname���o�^�ς݂��`�F�b�N
    if (paths_.count(nickname)) {
        // �o�^�ς݂̏ꍇ���O�o��
        Util::Log::PrintOutputWindow("[TextureManager] : This nickname is already in use. (" + nickname + ") The registration has therefore been cancelled.");

        return true;
    }
    return false;
}
