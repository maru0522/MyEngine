#include "PSOManager.h"
#include <cassert>
#include "Util.h"
#include "InitDirectX.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

void GraphicsPipeline::Create(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, size_t patternNum, Blob_t* blobsPtr, size_t  rootParamsCBNum, D3D12_CULL_MODE cullmode, BlendMode mode)
{
    //// �f�X�N���v�^�����W
    //CD3DX12_DESCRIPTOR_RANGE cDescRange{};
    //cDescRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
    //// ���[�g�p�����[�^
    //std::vector<D3D12_ROOT_PARAMETER> rootParams{};
    //CD3DX12_ROOT_PARAMETER cRootParam1{};
    //cRootParam1.InitAsDescriptorTable(1, &cDescRange); // �e�N�X�`�����W�X�^0��
    //rootParams.emplace_back(cRootParam1);

    //for (size_t i = 0; i < rootParamsCBNum; i++)
    //{
    //    CD3DX12_ROOT_PARAMETER cRootParamOfi{};
    //    cRootParamOfi.InitAsConstantBufferView((UINT)i); // �萔�o�b�t�@ 0~i ��
    //    rootParams.emplace_back(cRootParamOfi);
    //}

    // ���[�g�p�����[�^�R���e�i
    std::vector<D3D12_ROOT_PARAMETER> rootParams{};

    for (size_t i = 0; i < 1; i++)
    {
        // �f�X�N���v�^�����W
        CD3DX12_DESCRIPTOR_RANGE cDescRangeSRV{};
        cDescRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)i, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        // ���[�g�p�����[�^�����ƒǉ� - DescTable
        CD3DX12_ROOT_PARAMETER cRootParamDT{};
        cRootParamDT.InitAsDescriptorTable(1, &cDescRangeSRV, D3D12_SHADER_VISIBILITY_ALL); // �e�N�X�`�����W�X�^0��
        rootParams.emplace_back(cRootParamDT);
    }
    for (size_t i = 0; i < rootParamsCBNum; i++)
    {
        // ���[�g�p�����[�^�����ƒǉ� - ConstantBufferView
        CD3DX12_ROOT_PARAMETER cRootParamCBV{};
        cRootParamCBV.InitAsConstantBufferView((UINT)i); // �萔�o�b�t�@ 0~i ��
        rootParams.emplace_back(cRootParamCBV);
    }

    //// ���[�g�p�����[�^�R���e�i
    //std::vector<D3D12_ROOT_PARAMETER> rootParams{}; 
    //CreateRootParameter(rootParams,{ 1,rootParamsCBNum });

    //�e�N�X�`���T���v���[�̕ϐ��錾
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
    // ���[�g�V�O�l�`���f�X�N
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = CreateRootSignatureDesc(samplerDesc, patternNum, rootParams);
    // �V���A���C�Y���[�g�V�O�l�`���I�u�W�F�N�g
    ComPtr<ID3DBlob> rootSigBlob{ nullptr };
    // �V���A���C�Y���[�g�V�O�l�`��
    VSerializeRootSignature(rootSigBlob.GetAddressOf(), &rootSignatureDesc);
    // ���[�g�V�O�l�`������
    VCreateRootSignature(rootSigBlob.Get(), pipelineStateObject_.rootSignature.GetAddressOf());

    // �p�C�v���C���f�X�N
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = CreatePipelineDesc(blobsPtr, inputLayoutRef, pipelineStateObject_, cullmode, mode);
    // �p�C�v���C���X�e�[�g�I�u�W�F�N�g����
    VCreatePSO(&pipelineDesc, pipelineStateObject_.pipelineState.GetAddressOf());
}

void HelperGraphicPipeline::CreateRootParameter(std::vector<D3D12_ROOT_PARAMETER>& rootParams, RootParameterStructure_t rootParameterStructure)
{
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
}

D3D12_ROOT_SIGNATURE_DESC HelperGraphicPipeline::CreateRootSignatureDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, size_t patternNum, const std::vector<D3D12_ROOT_PARAMETER>& rootParamsRef)
{
    //�e�N�X�`���T���v���[�̐ݒ�
    SetSamplerDesc(samplerDescRef, patternNum);

    // ���[�g�V�O�l�`���f�X�N�̕ϐ��錾
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    // ���[�g�V�O�l�`���f�X�N�̐ݒ�
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParamsRef.data(); // ���[�g�p�����[�^�̐擪�A�h���X
    rootSignatureDesc.NumParameters = static_cast<uint32_t>(rootParamsRef.size());
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

D3D12_GRAPHICS_PIPELINE_STATE_DESC HelperGraphicPipeline::CreatePipelineDesc(Blob_t* blobsPtr, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, const PSO_t& psoRef, D3D12_CULL_MODE cullmode, BlendMode mode)
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
    switch (cullmode)
    {
    case D3D12_CULL_MODE_NONE:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
        break;
    case D3D12_CULL_MODE_FRONT:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT; // �J�����O���Ȃ�
        break;
    case D3D12_CULL_MODE_BACK:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // �J�����O���Ȃ�
        break;
    default:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
        break;
    }
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
    pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

    // �u�����h���[�h�ݒ�
    SetBlend(pipelineDesc, mode);

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

void HelperGraphicPipeline::SetSamplerDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, size_t patternNum)
{
    switch (patternNum)
    {
    case 1:
        //�e�N�X�`���T���v���[�̐ݒ�
        samplerDescRef.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�(�^�C�����O)
        samplerDescRef.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ��i�^�C�����O�j
        samplerDescRef.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
        samplerDescRef.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
        samplerDescRef.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//�S�ăV�j�A���
        samplerDescRef.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
        samplerDescRef.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
        samplerDescRef.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDescRef.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\
        break;
    case 2:
        //�e�N�X�`���T���v���[�̐ݒ�
        samplerDescRef.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//���J��Ԃ�(�^�C�����O)
        samplerDescRef.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//�c�J��Ԃ��i�^�C�����O�j
        samplerDescRef.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
        samplerDescRef.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
        samplerDescRef.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        samplerDescRef.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
        samplerDescRef.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
        samplerDescRef.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDescRef.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\
        break;
    default:
        //�e�N�X�`���T���v���[�̐ݒ�
        samplerDescRef.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�(�^�C�����O)
        samplerDescRef.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ��i�^�C�����O�j
        samplerDescRef.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
        samplerDescRef.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
        samplerDescRef.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//�S�ăV�j�A���
        samplerDescRef.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
        samplerDescRef.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
        samplerDescRef.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDescRef.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\
        break;
    }
}

void HelperGraphicPipeline::SetCompileShader(ID3DBlob** dp_blob, const std::string& filename, const std::string& entryPoint, const std::string& target)
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
    D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA�S�Ẵ`�����l����`��

    // ���ʐݒ�i�A���t�@�l�j
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;            // ���Z
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;              // �\�[�X�̒l��100%�g��
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;            // �f�X�g�̒l��0%�g��

    switch (mode)
    {
    case GraphicsPipeline::BlendMode::NONE:
        // �u�����h�𖳌��ɂ���
        blendDesc.BlendEnable = false;
        break;

    case GraphicsPipeline::BlendMode::ALPHA:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ����������
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // �\�[�X�̃A���t�@�l
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f - �\�[�X�̃A���t�@�l
        break;

    case GraphicsPipeline::BlendMode::ADDITIVE:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ���Z����(RGB�l)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // ���Z
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // �\�[�X�̒l��100%�g��
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // �f�X�g�̒l��100%�g��
        break;

    case GraphicsPipeline::BlendMode::SUBTRACTIVE:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ���Z����(RGB�l)
        blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;    // �f�X�g�J���\�[�X�����Z
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // �\�[�X�̒l��100%�g��
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // �f�X�g�̒l��100%�g��
        break;

    case GraphicsPipeline::BlendMode::MULTIPLICATION:
        // �u�����h��L���ɂ���
        blendDesc.BlendEnable = true;
        // ���Z����(RGB�l)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        break;

    case GraphicsPipeline::BlendMode::INVERSION:
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
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout1; // xyz���W,uv���W
    inputLayout1.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    inputLayout1.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    std::unique_ptr<Blob_t> blobs1{ std::make_unique<Blob_t>() };
    VCompileShaderVS(blobs1.get(), "SpriteVS.hlsl", "main");
    VCompileShaderPS(blobs1.get(), "SpritePS.hlsl", "main");
    CreateAllBlendType(inputLayout1, 1, blobs1.get(), 2, D3D12_CULL_MODE::D3D12_CULL_MODE_NONE, "PSO_SPRITE");

    // object3D
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout2; // xyz���W,�@���x�N�g��,uv���W
    inputLayout2.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    inputLayout2.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 });
    inputLayout2.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    std::unique_ptr<Blob_t> blobs2{ std::make_unique<Blob_t>() };
    VCompileShaderVS(blobs2.get(), "ModelVS.hlsl", "main");
    VCompileShaderPS(blobs2.get(), "ModelPS.hlsl", "main");
    CreateAllBlendType(inputLayout2, 1, blobs2.get(), 4, D3D12_CULL_MODE::D3D12_CULL_MODE_BACK, "PSO_OBJECT3D");

    // postEffect
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout3; // xyz���W,uv���W
    inputLayout3.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    inputLayout3.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    std::unique_ptr<Blob_t> blobs3{ std::make_unique<Blob_t>() };
    VCompileShaderVS(blobs3.get(), "PostEffectVS.hlsl", "main");
    VCompileShaderPS(blobs3.get(), "PostEffectPS.hlsl", "main");
    CreateAllBlendType(inputLayout3, 2, blobs3.get(), 1, D3D12_CULL_MODE::D3D12_CULL_MODE_NONE, "PSO_POSTEFFECT");
}

void PSOManager::CreateAllBlendType(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, size_t patternNum, Blob_t* blobsPtr, size_t  rootParamsCBNum, D3D12_CULL_MODE cullmode, const std::string& mapKey)
{
    std::array<GraphicsPipeline, static_cast<size_t>(HelperGraphicPipeline::BlendMode::MAX_ARRAY)> psoBlock;
    for (size_t i = 0; i < psoBlock.size(); i++)
    {
        psoBlock[i].Create(inputLayoutRef, patternNum, blobsPtr, rootParamsCBNum, cullmode, static_cast<BlendMode>(i));
    }

    pipelineStateObjects_.insert({ mapKey,psoBlock });
}
