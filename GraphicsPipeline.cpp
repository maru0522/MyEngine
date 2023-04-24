#include "GraphicsPipeline.h"
#include <cassert>
#include "Util.h"
#include "InitDirectX.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

GraphicsPipeline* GraphicsPipeline::GetInstance(void)
{
    static GraphicsPipeline instance{};

    return &instance;
}

const GraphicsPipeline::Pipeline_t& GraphicsPipeline::GetPipeline2d(BlendMode mode) const
{
    return pipelineProducts2d_.at(static_cast<int32_t>(mode));
}

void GraphicsPipeline::Initialize(void)
{
    for (size_t i = 0; i < pipelineProducts2d_.size(); i++) {
        Pipeline_t pipeline{};
        HelperGraphicPipeline::Pipeline2d(pipeline, static_cast<BlendMode>(i));

        pipelineProducts2d_.at(i) = pipeline;
    }

    //for (size_t i = 0; i < pipelineProducts3d_.size(); i++) {
    //    Pipeline_t pipeline{};
    //    HelperGraphicPipeline::Pipeline3d(pipeline, static_cast<BlendMode>(i));

    //    pipelineProducts3d_.at(i) = pipeline;
    //}
}

void HelperGraphicPipeline::Pipeline2d(Pipeline_t& pipeline, BlendMode mode)
{
    // ���_���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        {   // xyz���W
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        { // uv���W
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
    };

    // blob�錾
    std::unique_ptr<Blob_t> blobs{ std::make_unique<Blob_t>() };
    ComPtr<ID3DBlob> errorBlob{ nullptr }; // �G���[�I�u�W�F�N�g

    // ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
    SetCompileShader(blobs->vsBlob.GetAddressOf(), "SpriteVS.hlsl", "main", "vs_5_0");

    // �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
    SetCompileShader(blobs->psBlob.GetAddressOf(), "SpritePS.hlsl", "main", "ps_5_0");

#pragma region �p�C�v���C���̐ݒ�
    // �O���t�B�b�N�X�p�C�v���C���ݒ�
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

    // �V�F�[�_�[�̐ݒ�
    pipelineDesc.VS.pShaderBytecode = blobs->vsBlob->GetBufferPointer();
    pipelineDesc.VS.BytecodeLength = blobs->vsBlob->GetBufferSize();
    pipelineDesc.PS.pShaderBytecode = blobs->psBlob->GetBufferPointer();
    pipelineDesc.PS.BytecodeLength = blobs->psBlob->GetBufferSize();

    // �T���v���}�X�N�̐ݒ�
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

    // ���X�^���C�U�̐ݒ�
    pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
    pipelineDesc.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

#pragma region �u�����h�ݒ�
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
#pragma endregion

    // ���_���C�A�E�g�̐ݒ�
    pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
    pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

    // �}�`�̌`��ݒ�
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // ���̑��̐ݒ�
    pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
    pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

#pragma region �f�X�N���v�^�����W
    // �f�X�N���v�^�����W�̕ϐ��錾
    D3D12_DESCRIPTOR_RANGE descriptorRange{};
    // �f�X�N���v�^�����W�̐ݒ�
    descriptorRange.NumDescriptors = 1;
    descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange.BaseShaderRegister = 0; // �e�N�X�`�����W�X�^0��
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

#pragma region ���[�g�p�����[�^
    // ���[�g�p�����[�^�[�̐ݒ�
    D3D12_ROOT_PARAMETER rootParams[2] = {};
    // �e�N�X�`�����W�X�^0��
    rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// ���
    rootParams[0].DescriptorTable.pDescriptorRanges = &descriptorRange;
    rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    // �萔�o�b�t�@0��
    rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // �萔�o�b�t�@�r���[
    rootParams[1].Descriptor.ShaderRegister = 0;                    // �萔�o�b�t�@�ԍ�
    rootParams[1].Descriptor.RegisterSpace = 0;                     // �f�t�H���g�l
    rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   // �S�ẴV�F�[�_���猩����
    ////// �萔�o�b�t�@1��
    //rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // �萔�o�b�t�@�r���[
    //rootParams[2].Descriptor.ShaderRegister = 1;                    // �萔�o�b�t�@�ԍ�
    //rootParams[2].Descriptor.RegisterSpace = 0;                     // �f�t�H���g�l
    //rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   // �S�ẴV�F�[�_���猩����
#pragma endregion

#pragma region �T���v���[�̐ݒ�
    //�e�N�X�`���T���v���[�̕ϐ��錾
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
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
#pragma endregion

#pragma region ���[�g�V�O�l�`��
    // ���[�g�V�O�l�`���̕ϐ��錾
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    // ���[�g�V�O�l�`���̐ݒ�
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParams; // ���[�g�p�����[�^�̐擪�A�h���X
    rootSignatureDesc.NumParameters = static_cast<uint32_t>(std::size(rootParams));
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.NumStaticSamplers = 1;

    ComPtr<ID3DBlob> rootSigBlob{ nullptr }; // ���[�g�V�O�l�`���̃V���A���C�Y

    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
    assert(SUCCEEDED(hr));

    hr = InitDirectX::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline.rootSignature_));
    assert(SUCCEEDED(hr));
#pragma endregion

    // �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
    pipelineDesc.pRootSignature = pipeline.rootSignature_.Get();
#pragma endregion �p�C�v���C���̐ݒ�

    // �p�C�v���C���ݒ��ۑ�
    hr = InitDirectX::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipeline.pipelineState_));
    assert(SUCCEEDED(hr));
}

void HelperGraphicPipeline::Pipeline3d(Pipeline_t& pipeline, BlendMode mode)
{
    // ���_���C�A�E�g
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { // xyz���W(1�s�ŏ������ق������₷��)
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        { // �@���x�N�g���i1�s�������ق����킩��₷��)
            "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        },
        { // uv���W(1�s�ŏ������ق������₷��)
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
    };

    // blob�錾
    std::unique_ptr<Blob_t> blobs{ std::make_unique<Blob_t>() };
    ComPtr<ID3DBlob> errorBlob{ nullptr }; // �G���[�I�u�W�F�N�g

    // ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
    SetCompileShader(blobs->vsBlob.GetAddressOf(), "Resources/Shaders/ModelVS.hlsl", "main", "vs_5_0");

    // �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
    SetCompileShader(blobs->psBlob.GetAddressOf(), "Resources/Shaders/ModelPS.hlsl", "main", "ps_5_0");

#pragma region �p�C�v���C���̐ݒ�
    // �O���t�B�b�N�X�p�C�v���C���ݒ�
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

    // �V�F�[�_�[�̐ݒ�
    pipelineDesc.VS.pShaderBytecode = blobs->vsBlob->GetBufferPointer();
    pipelineDesc.VS.BytecodeLength = blobs->vsBlob->GetBufferSize();
    pipelineDesc.PS.pShaderBytecode = blobs->psBlob->GetBufferPointer();
    pipelineDesc.PS.BytecodeLength = blobs->psBlob->GetBufferSize();

    // �T���v���}�X�N�̐ݒ�
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

    // ���X�^���C�U�̐ݒ�
    pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;   // �w�ʂ��J�����O
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;  // �|���S�����h��Ԃ�
    pipelineDesc.RasterizerState.DepthClipEnable = true;            // �[�x�N���b�s���O��L����

#pragma region �u�����h�ݒ�
    // �����_�[�^�[�Q�b�g�̃u�����h�ݒ�i�u�����h�X�e�[�g�j�����ꃁ���o�ɂ������������񂶂�ˁH�H
    D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA�S�Ẵ`�����l����`��

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
#pragma endregion

    // ���_���C�A�E�g�̐ݒ�
    pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
    pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

    // �}�`�̌`��ݒ�
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // ���̑��̐ݒ�
    pipelineDesc.NumRenderTargets = 1; // �`��Ώۂ�1��
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255�w���RGBA
    pipelineDesc.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

#pragma region �f�X�N���v�^�����W
    // �f�X�N���v�^�����W�̐ݒ�
    D3D12_DESCRIPTOR_RANGE descriptorRange{};
    descriptorRange.NumDescriptors = 1;
    descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange.BaseShaderRegister = 0;	// �e�N�X�`�����W�X�^0��
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

#pragma region ���[�g�p�����[�^
    // ���[�g�p�����[�^�[�̐ݒ�
    D3D12_ROOT_PARAMETER rootParams[3] = {};
    // �萔�o�b�t�@0��
    rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// �萔�o�b�t�@�r���[
    rootParams[0].Descriptor.ShaderRegister = 0;					// �萔�o�b�t�@�ԍ�
    rootParams[0].Descriptor.RegisterSpace = 0;						// �f�t�H���g�l
    rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	// �S�ẴV�F�[�_���猩����
    // �萔�o�b�t�@1��
    rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// ���
    rootParams[1].Descriptor.ShaderRegister = 1;					// �萔�o�b�t�@�ԍ�
    rootParams[1].Descriptor.RegisterSpace = 0;						// �f�t�H���g�l
    rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	// �S�ẴV�F�[�_���猩����
    // �e�N�X�`�����W�X�^0��
    rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// ���
    rootParams[2].DescriptorTable.pDescriptorRanges = &descriptorRange;
    rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
    rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
#pragma endregion

#pragma region �T���v���[�̐ݒ�
    //�e�N�X�`���T���v���[�̐ݒ�
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ�(�^�C�����O)
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ��i�^�C�����O�j
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//�S�ăV�j�A���
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
    samplerDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_����̂ݎg�p�\
#pragma endregion

#pragma region ���[�g�V�O�l�`��
    //���[�g�V�O�l�`���̐ݒ�
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParams;//���[�g�p�����[�^�̐擪�A�h���X
    rootSignatureDesc.NumParameters = _countof(rootParams);//���[�g�p�����[�^��
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.NumStaticSamplers = 1;

    ComPtr<ID3DBlob> rootSigBlob{ nullptr }; // ���[�g�V�O�l�`���̃V���A���C�Y
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
    assert(SUCCEEDED(hr));

    hr = InitDirectX::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline.rootSignature_));
    assert(SUCCEEDED(hr));
#pragma endregion

    // �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
    pipelineDesc.pRootSignature = pipeline.rootSignature_.Get();

#pragma region �[�x�ݒ�
    // �f�v�X�X�e���V���X�e�[�g�̐ݒ�
    pipelineDesc.DepthStencilState.DepthEnable = true;  // �[�x�e�X�g���s��
    pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;     // �������݋���
    pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;          // ��������΍��i
    pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;                                 // �[�x�l�t�H�[�}�b�g
#pragma endregion
#pragma endregion

#pragma region �p�C�v���C���X�e�[�g�̐���
    // �p�C�v���C���ݒ�̕ۑ�
    hr = InitDirectX::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipeline.pipelineState_));
    assert(SUCCEEDED(hr));
#pragma endregion
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
