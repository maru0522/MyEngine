#pragma once
#include "InitDirectX.h"
#include <cassert>
#include <d3d12.h>
#include <vector>
#include "Util.h"
#include <wrl.h>

template<class T> class VertexBuffer
{
public:
    // �֐�
    VertexBuffer(void) = default;
    ~VertexBuffer(void) {/* buff_->Unmap(0, nullptr); */}

    void Create(const std::vector<T>& vertices) {
        // ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
        unsigned int sizeVB = static_cast<unsigned int>(sizeof(vertices[0]) * vertices.size());

        // ���_�o�b�t�@�̐ݒ�
        D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

        // ���\�[�X�ݒ�
        D3D12_RESOURCE_DESC resDesc{};
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.SampleDesc.Count = 1;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // ���_�o�b�t�@�̐���
        HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateCommittedResource(
            &heapProp, // �q�[�v�ݒ�
            D3D12_HEAP_FLAG_NONE,
            &resDesc, // ���\�[�X�ݒ�
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&buff_));
        assert(SUCCEEDED(hr));

        // GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
        hr = buff_->Map(0, nullptr, (void**)&vertMap_);
        assert(SUCCEEDED(hr));

        // �S���_�ɑ΂���
        TransferVertexToBuffer(vertices);

        // ���_�o�b�t�@�r���[
        // GPU���z�A�h���X
        vbView_.BufferLocation = buff_->GetGPUVirtualAddress();
        // ���_�o�b�t�@�̃T�C�Y
        vbView_.SizeInBytes = sizeVB;
        // ���_1���̃f�[�^�T�C�Y
        vbView_.StrideInBytes = sizeof(vertices[0]);
    }
    void TransferVertexToBuffer(const std::vector<T>& vertices) {
        // buffer�쐬�����A�傫��size��vertices���g�p���ɗ�O�X���[�Ȃ����s����N����\������B
        std::copy(vertices.begin(), vertices.end(), vertMap_); // �S���_�ɑ΂���
    }

private:
    // �ϐ�
    Microsoft::WRL::ComPtr<ID3D12Resource> buff_{}; // ���_�o�b�t�@
    D3D12_VERTEX_BUFFER_VIEW vbView_{};             // ���_�o�b�t�@�r���[
    T* vertMap_{};                                  // GPU��������map

public:
    // getter
    inline ID3D12Resource* GetBuffer(void) { return buff_.Get(); }
    inline const D3D12_VERTEX_BUFFER_VIEW& GetVbView(void) { return vbView_; }
    inline size_t GetVerticesNum(void) { return buff_->GetDesc().Width / sizeof(T); }
};
