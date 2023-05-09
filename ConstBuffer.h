#pragma once
#include <Windows.h>
#include <cassert>
#include <algorithm>
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "InitDirectX.h"

template<typename T> class ConstBuffer
{
public:
    // �֐�
    ConstBuffer(void) = default;
    ~ConstBuffer(void) = default;

    void Create() {
        // �萔�o�b�t�@�����p�̐ݒ�
        // �q�[�v�ݒ�
        D3D12_HEAP_PROPERTIES cbHeapProp{};
        cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;               // GPU�ւ̓]���p

        // ���\�[�X�ݒ�
        D3D12_RESOURCE_DESC cbResourceDesc{};
        cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        cbResourceDesc.Width = (sizeof(T) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
        cbResourceDesc.Height = 1;
        cbResourceDesc.DepthOrArraySize = 1;
        cbResourceDesc.MipLevels = 1;
        cbResourceDesc.SampleDesc.Count = 1;
        cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // �萔�o�b�t�@�̐���
        HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateCommittedResource(
            &cbHeapProp,    // �q�[�v�ݒ�
            D3D12_HEAP_FLAG_NONE,
            &cbResourceDesc,    // ���\�[�X�ݒ�
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&buffer_));
        assert(SUCCEEDED(hr));

        // �萔�o�b�t�@�̃}�b�s���O
        CBMap();
        //SetColor�͎g�p��ɈϔC
    }

private:
    // cBMaterial��cMapMaterial�̃}�b�s���O
    void CBMap(void) {
        // �萔�o�b�t�@�̃}�b�s���O
        HRESULT hr = buffer_->Map(0, nullptr, (void**)&cbTypeMap_);
        assert(SUCCEEDED(hr));
    }

    void CBUnMap(void) {
        buffer_->Unmap(0, nullptr);
    }

    // �ϐ�
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer_{};
    T* cbTypeMap_{};

public:
    // getter
    // private�ϐ�: cBTypeMap_ ���擾
    CBType* GetConstBuffMap(void) { return cbTypeMap_; } // �l���������p�擾�i�}�e���A���J���[�̃Z�b�e�B���O�Ƃ��j
    // private�ϐ�: cBMaterial ���擾
    ID3D12Resource* GetBuffer(void) { return buffer_.Get(); }
};