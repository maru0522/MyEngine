#include "PSOManager.h"
#include <cassert>
#include "Util.h"
#include "InitDirectX.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

void IGraphicsPipeline::Create(IGraphicsPipelineStructure_t* igpsPtr, BlendMode blendMode)
{
    // ���[�g�p�����[�^�R���e�i
    std::vector<CD3DX12_ROOT_PARAMETER> cRootParams{};
    cRootParams.reserve(igpsPtr->rps.descriptorRangeNum + igpsPtr->rps.rootParamsCBNum);

    std::vector<CD3DX12_DESCRIPTOR_RANGE> cDescRangeSRVs{};
    cDescRangeSRVs.reserve(igpsPtr->rps.descriptorRangeNum);
    for (size_t i = 0; i < igpsPtr->rps.descriptorRangeNum; i++)
    {
        // �f�X�N���v�^�����W
        cDescRangeSRVs.emplace_back();
        cDescRangeSRVs.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)i, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        // ���[�g�p�����[�^�����ƒǉ� - DescTable
        cRootParams.emplace_back();
        cRootParams.back().InitAsDescriptorTable(1, &cDescRangeSRVs.back(), D3D12_SHADER_VISIBILITY_ALL); // �e�N�X�`�����W�X�^0��
    }
    for (size_t i = 0; i < igpsPtr->rps.rootParamsCBNum; i++)
    {
        // ���[�g�p�����[�^�����ƒǉ� - ConstantBufferView
        cRootParams.emplace_back();
        cRootParams.back().InitAsConstantBufferView((UINT)i); // �萔�o�b�t�@ 0~i ��
    }

    //�e�N�X�`���T���v���[
    D3D12_STATIC_SAMPLER_DESC samplerDesc = CreateSamplerDesc(igpsPtr->samplerType); //SamplerType type
    // ���[�g�V�O�l�`���f�X�N
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = CreateRootSignatureDesc(samplerDesc, cRootParams);
    // �V���A���C�Y���[�g�V�O�l�`��
    ComPtr<ID3DBlob> rootSigBlob{ nullptr };
    VSerializeRootSignature(rootSigBlob.GetAddressOf(), &rootSignatureDesc);
    // ���[�g�V�O�l�`������
    VCreateRootSignature(rootSigBlob.Get(), pipelineStateObject_.rootSignature.GetAddressOf());

    // �p�C�v���C���f�X�N
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = CreatePipelineDesc(igpsPtr->blobs.get(), igpsPtr->inputLayouts, pipelineStateObject_, igpsPtr->cullMode, blendMode);
    // �p�C�v���C���X�e�[�g�I�u�W�F�N�g����
    VCreatePSO(&pipelineDesc, pipelineStateObject_.pipelineState.GetAddressOf());
}

std::vector<D3D12_ROOT_PARAMETER> HelperGraphicPipeline::CreateRootParameter(RootParameterStructure_t rootParameterStructure)
{
    // ���[�g�p�����[�^�R���e�i
    std::vector<D3D12_ROOT_PARAMETER> rootParams{};

    for (size_t i = 0; i < rootParameterStructure.descriptorRangeNum; i++)
    {
        // �f�X�N���v�^�����W
        CD3DX12_DESCRIPTOR_RANGE cDescRangeSRV{};
        cDescRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)i, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        // ���[�g�p�����[�^�����ƒǉ� - DescTable
        CD3DX12_ROOT_PARAMETER cRootParamDT{};
        cRootParamDT.InitAsDescriptorTable(1, &cDescRangeSRV, D3D12_SHADER_VISIBILITY_ALL); // �e�N�X�`�����W�X�^0��
        rootParams.emplace_back(cRootParamDT);
    }
    for (size_t i = 0; i < rootParameterStructure.rootParamsCBNum; i++)
    {
        // ���[�g�p�����[�^�����ƒǉ� - ConstantBufferView
        CD3DX12_ROOT_PARAMETER cRootParamCBV{};
        cRootParamCBV.InitAsConstantBufferView((UINT)i); // �萔�o�b�t�@ 0~i ��
        rootParams.emplace_back(cRootParamCBV);
    }
    return rootParams;
}

D3D12_STATIC_SAMPLER_DESC HelperGraphicPipeline::CreateSamplerDesc(SamplerType type)
{
    //�e�N�X�`���T���v���[�̕ϐ��錾
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};

    switch (static_cast<int>(type))
    {
    case 0:
        //�e�N�X�`���T���v���[�̐ݒ�
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�(�^�C�����O)
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ��i�^�C�����O�j
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
        samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//�S�ăV�j�A���
        samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
        samplerDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\
        break;
    case 1:
        //�e�N�X�`���T���v���[�̐ݒ�
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//���J��Ԃ�(�^�C�����O)
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//�c�J��Ԃ��i�^�C�����O�j
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
        samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
        samplerDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\
        break;
    default:
        //�e�N�X�`���T���v���[�̐ݒ�
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�(�^�C�����O)
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ��i�^�C�����O�j
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
        samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//�S�ăV�j�A���
        samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
        samplerDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\
        break;
    }

    return samplerDesc;
}

D3D12_ROOT_SIGNATURE_DESC HelperGraphicPipeline::CreateRootSignatureDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, const std::vector<CD3DX12_ROOT_PARAMETER>& cRootParamsRef)
{
    // ���[�g�V�O�l�`���f�X�N�̕ϐ��錾
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    // ���[�g�V�O�l�`���f�X�N�̐ݒ�
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = cRootParamsRef.data(); // ���[�g�p�����[�^�̐擪�A�h���X
    rootSignatureDesc.NumParameters = static_cast<uint32_t>(cRootParamsRef.size());
    rootSignatureDesc.pStaticSamplers = &samplerDescRef;
    rootSignatureDesc.NumStaticSamplers = 1;

    return rootSignatureDesc;
}

void HelperGraphicPipeline::VSerializeRootSignature(ID3DBlob** rootSigBlobDPtr, D3D12_ROOT_SIGNATURE_DESC* rootSignatureDescPtr)
{
    // �G���[�I�u�W�F�N�g
    ComPtr<ID3DBlob> errorBlob{ nullptr };
    HRESULT hr = D3D12SerializeRootSignature(rootSignatureDescPtr, D3D_ROOT_SIGNATURE_VERSION_1_0, rootSigBlobDPtr, &errorBlob);
    assert(SUCCEEDED(hr));
}

void HelperGraphicPipeline::VCreateRootSignature(ID3DBlob* rootSigBlobPtr, ID3D12RootSignature** rootSignatureDPtr)
{
    HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlobPtr->GetBufferPointer(), rootSigBlobPtr->GetBufferSize(), IID_PPV_ARGS(rootSignatureDPtr));
    assert(SUCCEEDED(hr));
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC HelperGraphicPipeline::CreatePipelineDesc(Blob_t* blobsPtr, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, const PSO_t& psoRef, D3D12_CULL_MODE cullMode, BlendMode blendMode)
{
    // �O���t�B�b�N�X�p�C�v���C���ݒ�
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

    // �V�F�[�_�[�̐ݒ�
    if (blobsPtr->vsBlob.Get()) { pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(blobsPtr->vsBlob.Get()); }
    if (blobsPtr->gsBlob.Get()) { pipelineDesc.GS = CD3DX12_SHADER_BYTECODE(blobsPtr->gsBlob.Get()); }
    if (blobsPtr->psBlob.Get()) { pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(blobsPtr->psBlob.Get()); }

    // �T���v���}�X�N�̐ݒ�
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

    // ���X�^���C�U�̐ݒ�
    switch (cullMode)
    {
    case D3D12_CULL_MODE_NONE:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
        break;
    case D3D12_CULL_MODE_FRONT:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
        break;
    case D3D12_CULL_MODE_BACK:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        break;
    default:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
        break;
    }
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
    pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

    // �u�����h���[�h�ݒ�
    SetBlend(pipelineDesc, blendMode);

    // ���_���C�A�E�g�̐ݒ�
    pipelineDesc.InputLayout.pInputElementDescs = inputLayoutRef.data();
    pipelineDesc.InputLayout.NumElements = (UINT)inputLayoutRef.size();

    // �}�`�̌`��ݒ�
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // ���̑��̐ݒ�
    pipelineDesc.NumRenderTargets = 2; // �`��Ώۂ�1��
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
    pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
    pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

    // �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
    pipelineDesc.pRootSignature = psoRef.rootSignature.Get();

    return pipelineDesc;
}

void HelperGraphicPipeline::VCreatePSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineDescPtr, ID3D12PipelineState** psoDPtr)
{
    // �p�C�v���C���ݒ��ۑ�
    HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateGraphicsPipelineState(pipelineDescPtr, IID_PPV_ARGS(psoDPtr));
    assert(SUCCEEDED(hr));
}

void HelperGraphicPipeline::VSetCompileShader(ID3DBlob** dp_blob, const std::string& filename, const std::string& entryPoint, const std::string& target)
{
    std::wstring wFilename{ Util::Convert::ToWString(filename) };

    // �G���[�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob{ nullptr };

    // ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
    HRESULT hr = D3DCompileFromFile(wFilename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, dp_blob, &errorBlob);

    // �G���[�Ȃ�
    if (FAILED(hr)) {
        // errorBlob����G���[���e��string�^�ɃR�s�[
        std::string error;
        error.resize(errorBlob->GetBufferSize());
        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            error.begin());
        error += "\n";
        // �G���[���e���o�̓E�B���h�E�ɕ\��
        OutputDebugStringA(error.c_str());
        assert(0);
    }
}

void HelperGraphicPipeline::SetBlend(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipelineDesc, BlendMode mode)
{
    // �����_�[�^�[�Q�b�g�̃u�����h�ݒ�i�u�����h�X�e�[�g�j
    D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��

    // ���ʐݒ�i�A���t�@�l�j
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;            // ���Z
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;              // �\�[�X�̒l��100%�g��
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;            // �f�X�g�̒l��0%�g��

    switch (mode)
    {
    case IGraphicsPipeline::BlendMode::NONE:
        // �u�����h�𖳌��ɂ���
        blendDesc.BlendEnable = false;
        break;

    case IGraphicsPipeline::BlendMode::ALPHA:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ����������
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // �\�[�X�̃A���t�@�l
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f - �\�[�X�̃A���t�@�l
        break;

    case IGraphicsPipeline::BlendMode::ADDITIVE:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ���Z����(RGB�l)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // �\�[�X�̒l��100%�g��
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // �f�X�g�̒l��100%�g��
        break;

    case IGraphicsPipeline::BlendMode::SUBTRACTIVE:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ���Z����(RGB�l)
        blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;    // �f�X�g�J���\�[�X�����Z
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // �\�[�X�̒l��100%�g��
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // �f�X�g�̒l��100%�g��
        break;

    case IGraphicsPipeline::BlendMode::MULTIPLICATION:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ���Z����(RGB�l)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        break;

    case IGraphicsPipeline::BlendMode::INVERSION:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // �F���](RGB�l)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
        blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;    // 1.0f - �f�X�g�J���[�̒l
        blendDesc.DestBlend = D3D12_BLEND_ZERO;             // �g��Ȃ�
        break;

    default:
        // �u�����h�𖳌��ɂ���
        blendDesc.BlendEnable = false;
        break;
    }

    pipelineDesc.BlendState.RenderTarget[0] = blendDesc;
    pipelineDesc.BlendState.RenderTarget[1] = blendDesc;
}

PSOManager* PSOManager::GetInstance(void)
{
    static PSOManager sInstance{};
    return &sInstance;
}

void PSOManager::Create()
{
    // sprite
    AddPipeline("PSO_SPRITE");
    GetPSOBlendPtr("PSO_SPRITE")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_SPRITE")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_SPRITE")->CompileShader(ShaderType::VS, "SpriteVS.hlsl", "main");
    GetPSOBlendPtr("PSO_SPRITE")->CompileShader(ShaderType::PS, "SpritePS.hlsl", "main");
    GetPSOBlendPtr("PSO_SPRITE")->SetRootParameterStructure(1, 2);
    GetPSOBlendPtr("PSO_SPRITE")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("PSO_SPRITE")->SetSamplerType(SamplerType::NORMAL);
    SetComplete("PSO_SPRITE");

    // object3D
    AddPipeline("PSO_OBJECT3D");
    GetPSOBlendPtr("PSO_OBJECT3D")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_OBJECT3D")->AddInputLayout({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 });
    GetPSOBlendPtr("PSO_OBJECT3D")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_OBJECT3D")->CompileShader(ShaderType::VS, "ModelVS.hlsl", "main");
    GetPSOBlendPtr("PSO_OBJECT3D")->CompileShader(ShaderType::PS, "ModelPS.hlsl", "main");
    GetPSOBlendPtr("PSO_OBJECT3D")->SetRootParameterStructure(1, 4);
    GetPSOBlendPtr("PSO_OBJECT3D")->SetCullMode(D3D12_CULL_MODE_BACK);
    GetPSOBlendPtr("PSO_OBJECT3D")->SetSamplerType(SamplerType::NORMAL);
    SetComplete("PSO_OBJECT3D");

    // postEffect
    AddPipeline("PSO_POSTEFFECT");
    GetPSOBlendPtr("PSO_POSTEFFECT")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_POSTEFFECT")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_POSTEFFECT")->CompileShader(ShaderType::VS, "PostEffectVS.hlsl", "main");
    GetPSOBlendPtr("PSO_POSTEFFECT")->CompileShader(ShaderType::PS, "PostEffectPS.hlsl", "main");
    GetPSOBlendPtr("PSO_POSTEFFECT")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("PSO_POSTEFFECT")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("PSO_POSTEFFECT")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("PSO_POSTEFFECT");

    // gaussianblur
    AddPipeline("GAUSSIAN");
    GetPSOBlendPtr("GAUSSIAN")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("GAUSSIAN")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("GAUSSIAN")->CompileShader(ShaderType::VS, "GaussianBlurVS.hlsl", "main");
    GetPSOBlendPtr("GAUSSIAN")->CompileShader(ShaderType::PS, "GaussianBlurPS.hlsl", "main");
    GetPSOBlendPtr("GAUSSIAN")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("GAUSSIAN")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("GAUSSIAN")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("GAUSSIAN");

    // extractHiLumi
    AddPipeline("HIGHLUMI");
    GetPSOBlendPtr("HIGHLUMI")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("HIGHLUMI")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("HIGHLUMI")->CompileShader(ShaderType::VS, "HighLumiVS.hlsl", "main");
    GetPSOBlendPtr("HIGHLUMI")->CompileShader(ShaderType::PS, "HighLumiPS.hlsl", "main");
    GetPSOBlendPtr("HIGHLUMI")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("HIGHLUMI")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("HIGHLUMI")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("HIGHLUMI");

    // bloom
    AddPipeline("BLOOM");
    GetPSOBlendPtr("BLOOM")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("BLOOM")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("BLOOM")->CompileShader(ShaderType::VS, "BloomVS.hlsl", "main");
    GetPSOBlendPtr("BLOOM")->CompileShader(ShaderType::PS, "BloomPS.hlsl", "main");
    GetPSOBlendPtr("BLOOM")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("BLOOM")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("BLOOM")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("BLOOM");
}

void PSOManager::GraphicsPipelineBlend_t::Create(void)
{
    for (size_t i = 0; i < static_cast<size_t>(BlendMode::MAX_ARRAY); i++)
    {
        blends[i].Create(&igps, static_cast<BlendMode>(i));
    }
}

void PSOManager::GraphicsPipelineBlend_t::CompileShader(ShaderType shaderType, const std::string& filename, const std::string& entryPoint)
{
    if (shaderType == ShaderType::VS) VSetCompileShader(igps.blobs->vsBlob.GetAddressOf(), filename, entryPoint, "vs_5_0");
    if (shaderType == ShaderType::GS) VSetCompileShader(igps.blobs->gsBlob.GetAddressOf(), filename, entryPoint, "gs_5_0");
    if (shaderType == ShaderType::PS) VSetCompileShader(igps.blobs->psBlob.GetAddressOf(), filename, entryPoint, "ps_5_0");
}

void PSOManager::GraphicsPipelineBlend_t::SetRootParameterStructure(size_t descriptorRangeNum, size_t rootParamsCBNum)
{
    igps.rps.descriptorRangeNum = descriptorRangeNum;
    igps.rps.rootParamsCBNum = rootParamsCBNum;
}
