#include <cassert>
#include <vector>

#include "InitDirectX.h"

InitDirectX* InitDirectX::GetInstance(void)
{
    static InitDirectX iDX;
    return &iDX;
}

void InitDirectX::Initialize(WndAPI* p_wndapi)
{
    // FPS�Œ艻����������
    fpsController_->Initialize();

    DebugLayer();       // �f�o�b�O���C���[���I��
#ifdef _DEBUG
    DXGIDevice();
    SuppressErrors();   // �ꕔ�̃G���[��}��
#endif
    Commands();
    SwapChain(p_wndapi);
    RTVDescHeap();
    RTV();
    DescriptorHeap();
    DepthBuffer();
    Fence();
}

void InitDirectX::PreDraw(void)
{
#pragma region ���\�[�X�o���A�̕ύX
    // �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
    UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

    // �P�D���\�[�X�o���A�ŏ������݉\�ɕύX
    D3D12_RESOURCE_BARRIER barrierDesc{};
    barrierDesc.Transition.pResource = backBuffers_[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
    commandList_->ResourceBarrier(1, &barrierDesc);
#pragma endregion

#pragma region �`���̕ύX
    // �Q�D�`���̕ύX
    // �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvDescHeapDesc_.Type);

    // �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

    // �����_�[�^�[�Q�b�g���Z�b�g
    commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
#pragma endregion

#pragma region ��ʃN���A
    // �R�D��ʃN���A 
    ClearRTV();

    // �[�x�o�b�t�@�N���A
    ClearDepthBuff();
#pragma endregion

#pragma region �O���t�B�b�N�X�R�}���h���͑O�ݒ�
    // �S�D�`��R�}���h��������
    // �r���[�|�[�g�ݒ�R�}���h
    D3D12_VIEWPORT viewport{};
    viewport.Width = WndAPI::kWidth_;
    viewport.Height = WndAPI::kHeight_;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    // �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
    commandList_->RSSetViewports(1, &viewport);

    // �V�U�[��`
    D3D12_RECT scissorRect{};
    scissorRect.left = 0; // �؂蔲�����W��
    scissorRect.right = scissorRect.left + WndAPI::kWidth_; // �؂蔲�����W�E
    scissorRect.top = 0; // �؂蔲�����W��
    scissorRect.bottom = scissorRect.top + WndAPI::kHeight_; // �؂蔲�����W��
    // �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
    commandList_->RSSetScissorRects(1, &scissorRect);
#pragma endregion
    // ----- ������PreDraw()��PostDraw()�̊Ԃɕ`��R�}���h���������ށ����� ----- //
}

void InitDirectX::PostDraw(void)
{
    // ----- ������PreDraw()��PostDraw()�̊Ԃɕ`��R�}���h���������ށ����� ----- //
    // �S�D�`��R�}���h�����܂�

    // �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
    UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

#pragma region ���\�[�X�o���A�����ɖ߂�
    // �T�D���\�[�X�o���A��߂�
    D3D12_RESOURCE_BARRIER barrierDesc{};
    barrierDesc.Transition.pResource = backBuffers_[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
    barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ�
    commandList_->ResourceBarrier(1, &barrierDesc);
#pragma endregion

#pragma region �R�}���h���X�g���s
    // ���߂̃N���[�Y
    HRESULT hr = commandList_->Close();
    assert(SUCCEEDED(hr));

    // �R�}���h���X�g�̎��s
    std::vector<ID3D12CommandList*> commandLists = { commandList_.Get() };
    commandQueue_->ExecuteCommandLists(1, commandLists.data());
    // ��ʂɕ\������o�b�t�@���t���b�v�i���\�̓��ւ��j
    hr = swapChain_->Present(1, 0);
    assert(SUCCEEDED(hr));

    // �R�}���h�̎��s������҂�
    commandQueue_->Signal(fence_.Get(), ++fenceVal_);
    if (fence_->GetCompletedValue() != fenceVal_) {
        HANDLE event = CreateEvent(nullptr, false, false, nullptr);
        if (event != 0) {
            fence_->SetEventOnCompletion(fenceVal_, event);
            WaitForSingleObject(event, INFINITE);
            CloseHandle(event);
        }
    }

    // FPS�Œ�
    fpsController_->Update();

    // �L���[���N���A
    hr = cmdAllocator_->Reset();
    assert(SUCCEEDED(hr));


    // �ĂуR�}���h���X�g�𒙂߂鏀��
    hr = commandList_->Reset(cmdAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr));
#pragma endregion
}

void InitDirectX::DebugLayer(void)
{
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
        debugController_->EnableDebugLayer();
        debugController_->SetEnableGPUBasedValidation(true);
    }
}

void InitDirectX::DXGIDevice(void)
{
#pragma region �A�_�v�^�̗�
    // DXGI�t�@�N�g���[�̐���
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

    assert(SUCCEEDED(hr));

    // �A�_�v�^�[�̗񋓗p
    std::vector<ComPtr<IDXGIAdapter4>> adapters;

    // �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
    ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

    // �p�t�H�[�}���X���������̂��珇�ɁA�S�ẴA�_�v�^�[��񋓂���
    for (UINT i = 0;
        dxgiFactory_->EnumAdapterByGpuPreference( // �����̈�����UINT�̂��� i �̌^��size_t�łȂ�
            i,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
        i++) {

        // ���I�z��ɒǉ�����
        adapters.push_back(tmpAdapter);
    }
#pragma endregion

#pragma region �A�_�v�^�̑I��
    // �Ó��ȃA�_�v�^��I�ʂ���
    for (size_t i = 0; i < adapters.size(); i++) {
        DXGI_ADAPTER_DESC3 adapterDesc;

        // �A�_�v�^�[�̏����擾����
        adapters.at(i)->GetDesc3(&adapterDesc);

        // �\�t�g�E�F�A�f�o�C�X�����
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // �f�o�C�X���̗p���ă��[�v�𔲂���
            tmpAdapter = adapters.at(i);

            break;
        }
    }
#pragma endregion

#pragma region �f�o�C�X�̐���
    // �Ή����x���̔z��
    std::vector<D3D_FEATURE_LEVEL> levels{
     D3D_FEATURE_LEVEL_12_1,
     D3D_FEATURE_LEVEL_12_0,
     D3D_FEATURE_LEVEL_11_1,
     D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;
    for (size_t i = 0; i < levels.size(); i++) {
        // �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
        hr = D3D12CreateDevice(tmpAdapter.Get(), levels.at(i), IID_PPV_ARGS(&device_));

        if (hr == S_OK) {
            // �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
            featureLevel = levels.at(i);
            break;
        }
    }
#pragma endregion
}

void InitDirectX::SuppressErrors(void)
{
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
        infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);    //��΂��G���[�̎��~�܂�
        infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);        //�G���[�̎��~�܂�
    }

    //�}������G���[ 
    std::vector<D3D12_MESSAGE_ID> denyIds = {
        D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
    };

    //�}�������\�����x��
    std::vector<D3D12_MESSAGE_SEVERITY> severities = { D3D12_MESSAGE_SEVERITY_INFO };
    D3D12_INFO_QUEUE_FILTER filter{};
    filter.DenyList.NumIDs = (UINT)denyIds.size();
    filter.DenyList.pIDList = denyIds.data();
    filter.DenyList.NumSeverities = (UINT)severities.size();
    filter.DenyList.pSeverityList = severities.data();
    //�w�肵���G���[�̕\����}������
    infoQueue_->PushStorageFilter(&filter);
}

void InitDirectX::Commands(void)
{
#pragma region �R�}���h�A���P�[�^
    // �R�}���h�A���P�[�^�𐶐�
    HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator_));

    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region �R�}���h���X�g�̐���
    // �R�}���h���X�g�𐶐�
    hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));

    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region �R�}���h�L���[�̐���
    //�R�}���h�L���[�̐ݒ�
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

    //�R�}���h�L���[�𐶐�
    hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

    assert(SUCCEEDED(hr));
#pragma endregion
}

void InitDirectX::SwapChain(WndAPI* p_wndapi)
{
    // �X���b�v�`�F�[���̐ݒ�
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = 1280;
    swapChainDesc.Height = 720;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �F���̏���
    swapChainDesc.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
    swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // �o�b�N�o�b�t�@�p
    swapChainDesc.BufferCount = 2; // �o�b�t�@�����Q�ɐݒ�
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �t���b�v��͔j��
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // IDXGISwapChain1��tmp
    ComPtr<IDXGISwapChain1> swapChain1Tmp;

    // �X���b�v�`�F�[���̐���
    HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), p_wndapi->GetHwnd(), &swapChainDesc, nullptr, nullptr, &swapChain1Tmp);
    assert(SUCCEEDED(hr));

    // ��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
    swapChain1Tmp.As(&swapChain_);
}

void InitDirectX::RTVDescHeap(void)
{
    // swapChainDesc1 �� ��ϐ���錾
    DXGI_SWAP_CHAIN_DESC swcd{};

    // �R�s�[�Ŏ擾
    HRESULT hr = swapChain_->GetDesc(&swcd);
    assert(SUCCEEDED(hr));

    // RTVDescHeap
    // �f�X�N���v�^�q�[�v�̐ݒ�
    rtvDescHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
    rtvDescHeapDesc_.NumDescriptors = swcd.BufferCount; // ���\�̂Q��

    // �ݒ������RTV�p�f�X�N���v�^�q�[�v�̐���
    device_->CreateDescriptorHeap(&rtvDescHeapDesc_, IID_PPV_ARGS(&rtvHeap_));
}

void InitDirectX::RTV(void)
{
    // swapChainDesc1 �� ��ϐ���錾
    DXGI_SWAP_CHAIN_DESC swcd{};

    // �R�s�[�Ŏ擾
    HRESULT hr = swapChain_->GetDesc(&swcd);
    assert(SUCCEEDED(hr));

    // RTV
    backBuffers_.resize(swcd.BufferCount);

    // �X���b�v�`�F�[���̑S�Ẵo�b�t�@�ɂ��ď�������
    for (size_t i = 0; i < backBuffers_.size(); i++) {
        // �X���b�v�`�F�[������o�b�t�@���擾
        swapChain_->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&backBuffers_.at(i)));
        // �f�X�N���v�^�q�[�v�̃n���h�����擾
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
        // �����\���ŃA�h���X�������
        rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvDescHeapDesc_.Type);
        // �����_�[�^�[�Q�b�g�r���[�̐ݒ�
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        // �V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        // �����_�[�^�[�Q�b�g�r���[�̐���
        device_->CreateRenderTargetView(backBuffers_.at(i).Get(), &rtvDesc, rtvHandle);
    }
}

void InitDirectX::DescriptorHeap(void)
{
    descHeap_ = std::make_unique<DescriptorHeap_t>(this);
}

void InitDirectX::DepthBuffer(void)
{
    //�[�x�e�X�g�[�x�o�b�t�@

    // ���\�[�X�ݒ�
    D3D12_RESOURCE_DESC depthResourceDesc{};
    depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthResourceDesc.Width = WndAPI::kWidth_;     // �����_�[�^�[�Q�b�g�ɍ��킹��
    depthResourceDesc.Height = WndAPI::kHeight_;   // �����_�[�^�[�Q�b�g�ɍ��킹��
    depthResourceDesc.DepthOrArraySize = 1;
    depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;   // �[�x�l�t�H�[�}�b�g
    depthResourceDesc.SampleDesc.Count = 1;
    depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;  // �f�v�X�X�e���V��

    // �[�x�l�p�q�[�v�v���p�e�B
    D3D12_HEAP_PROPERTIES depthHeapProp{};
    depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

    // �[�x�l�̃N���A�ݒ�
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;      // �[�x�l1.0f�i�ő�l�j�ŃN���A
    depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; // �[�x�l�t�H�[�}�b�g

    // �[�x�o�b�t�@�̐���
    // ���\�[�X����
    HRESULT hr = device_->CreateCommittedResource(
        &depthHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &depthResourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,       // �[�x�l�������݂Ɏg�p
        &depthClearValue,
        IID_PPV_ARGS(&depthBuff_)
    );
    assert(SUCCEEDED(hr));

    // �[�x�r���[�p�f�X�N���v�^�q�[�v�̐ݒ�
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;     // �[�x�r���[��1��
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;      // �f�v�X�X�e���V���r���[
    // �ݒ�����ɐ[�x�r���[�p�f�X�N���v�^�q�[�v�̐���
    hr = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
    assert(SUCCEEDED(hr));

    // �[�x�r���[�쐬
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;     // �[�x�l�t�H�[�}�b�g
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    // �[�x�r���[�̐���
    device_->CreateDepthStencilView(depthBuff_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
}

void InitDirectX::Fence(void)
{
    // �t�F���X�̐���
    HRESULT hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));
}

void InitDirectX::ClearRTV(void)
{
    // �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
    UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

    // �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvDescHeapDesc_.Type);

    // ���F�ŃN���A����̂��ݒ�
    std::vector<FLOAT> clearColor = { 0.1f,0.25f,0.5f,0.f };
    // RTV���N���A����֐����R�}���h���X�g�ɐς�
    commandList_->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);
}

void InitDirectX::ClearDepthBuff(void)
{
    // �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

    // �[�x�o�b�t�@���N���A����֐����R�}���h���X�g�ɐς�
    commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

InitDirectX::DescriptorHeap_t::DescriptorHeap_t(InitDirectX* p_idx)
    : p_idx_(p_idx)
{
    // �f�X�N���v�^�q�[�v�̐ݒ�
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// �V�F�[�_���猩����悤��
    descHeapDesc.NumDescriptors = maxSRVDesc_ + maxCBVDesc_ + maxUAVDesc_;

    // �ݒ������SRV�p�f�X�N���v�^�q�[�v���쐬
    HRESULT hr = p_idx_->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
    assert(SUCCEEDED(hr));
}

size_t InitDirectX::DescriptorHeap_t::CreateSRV(const D3D12_RESOURCE_DESC& rscDesc, ID3D12Resource* p_rsc)
{
    assert(SRVCount_ < maxSRVDesc_);

    // �f�X�N���v�^�̃T�C�Y
    size_t incrementSize = p_idx_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // cpu��descriptor�̃q�[�v�̃X�^�[�g�ʒu
    D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_{ descriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart() };
    // �n���h����i�߂�
    srvCpuHandle_.ptr += incrementSize * SRVCount_;

    // gpu��descriptor�̃q�[�v�̃X�^�[�g�ʒu
    D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_{ descriptorHeap_.Get()->GetGPUDescriptorHandleForHeapStart() };
    // �n���h����i�߂�
    srvGpuHandle_.ptr += incrementSize * SRVCount_;


    // �V�F�[�_���\�[�X�r���[�ݒ�
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = rscDesc.Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = rscDesc.MipLevels;

    // �n���h���̂����ʒu�ɃV�F�[�_�[���\�[�X�r���[�̍쐬
    p_idx_->GetDevice()->CreateShaderResourceView(p_rsc, &srvDesc, srvCpuHandle_);

    // SRV�𐶐����������L�^
    SRVCount_++;

    return srvGpuHandle_.ptr;
}
