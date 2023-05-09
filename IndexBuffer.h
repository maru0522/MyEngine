#pragma once
#include "InitDirectX.h"
#include <cassert>
#include <d3d12.h>
#include <vector>
#include "Util.h"
#include <wrl.h>

class IndexBuffer
{
public:
    // �֐�
    IndexBuffer(void) = default;
    ~IndexBuffer(void) {}

    void Create(const std::vector<unsigned short>& indices) {
        // �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
        unsigned int sizeIB = static_cast<unsigned int>(sizeof(unsigned short) * indices.size());

        // �C���f�b�N�X�o�b�t�@�̐ݒ�
        D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

        // ���\�[�X�ݒ�
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = sizeIB; // �C���f�b�N�X��񂪓��镪�̃T�C�Y
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // �C���f�b�N�X�o�b�t�@�̐���
        HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateCommittedResource(
            &heapProp, // �q�[�v�ݒ�
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // ���\�[�X�ݒ�
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&buff_));
        assert(SUCCEEDED(hr));

        // �}�b�s���O
        unsigned short* indexMap_{ nullptr }; // GPU��������map
        hr = buff_->Map(0, nullptr, (void**)&indexMap_);
        assert(SUCCEEDED(hr));

        std::copy(indices.begin(), indices.end(), indexMap_); // �S�C���f�b�N�X�ɑ΂���

        // �}�b�s���O����
        buff_->Unmap(0, nullptr);

        // �C���f�N�X�o�b�t�@�r���[
        // GPU���z�A�h���X
        ibView_.BufferLocation = buff_->GetGPUVirtualAddress();
        ibView_.Format = DXGI_FORMAT_R16_UINT;
        // �C���f�b�N�X�o�b�t�@�̃T�C�Y
        ibView_.SizeInBytes = sizeIB;
    }

private:
    // �ϐ�
    Microsoft::WRL::ComPtr<ID3D12Resource> buff_{}; // ���_�o�b�t�@
    D3D12_INDEX_BUFFER_VIEW ibView_{};              // ���_�o�b�t�@�r���[

public:
    // getter
    inline ID3D12Resource* GetBuffer(void) { return buff_.Get(); }
    inline const D3D12_INDEX_BUFFER_VIEW& GetIbView(void) { return ibView_; }
    inline size_t GetIndicesNum(void) { return buff_->GetDesc().Width / sizeof(unsigned short); }
};