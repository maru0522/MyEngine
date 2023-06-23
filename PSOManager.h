#pragma once
#include <map>
#include <wrl.h>
#include <array>
#include <string>
#include <d3d12.h>
#include <d3dx12.h>
#pragma comment(lib,"d3d12.lib")

class HelperGraphicPipeline
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    enum class BlendMode
    {
        // ����
        NONE,
        // ����������
        ALPHA,
        // ���Z����
        ADDITIVE,
        // ���Z����
        SUBTRACTIVE,
        // ��Z����
        MULTIPLICATION,
        // �F���]
        INVERSION,

        MAX_ARRAY,
    };

protected:
    enum class ShaderType
    {
        VS,
        GS,
        PS,
    };

    enum class SamplerType
    {
        NORMAL,
        POSTEFFECT,
    };

    struct PSO_t
    {
        // ���[�g�V�O�l�`��
        ComPtr<ID3D12RootSignature> rootSignature{};
        // �p�C�v�����X�e�[�g�̐���
        ComPtr<ID3D12PipelineState> pipelineState{};
    };

    struct Blob_t
    {
        // ���_�V�F�[�_�I�u�W�F�N�g
        ComPtr<ID3DBlob> vsBlob{ nullptr };
        // �W�I���g���V�F�[�_�I�u�W�F�N�g
        ComPtr<ID3DBlob> gsBlob{ nullptr };
        // �s�N�Z���V�F�[�_�I�u�W�F�N�g
        ComPtr<ID3DBlob> psBlob{ nullptr };
    };

    struct RootParameterStructure_t
    {
        size_t descriptorRangeNum{};
        size_t rootParamsCBNum{};
    };

    static inline void VCompileShaderVS(Blob_t* blobPtr, const std::string& filename, const std::string& entryPoint) { SetCompileShader(blobPtr->vsBlob.GetAddressOf(), filename, entryPoint, "vs_5_0"); }
    static inline void VCompileShaderGS(Blob_t* blobPtr, const std::string& filename, const std::string& entryPoint) { SetCompileShader(blobPtr->gsBlob.GetAddressOf(), filename, entryPoint, "gs_5_0"); }
    static inline void VCompileShaderPS(Blob_t* blobPtr, const std::string& filename, const std::string& entryPoint) { SetCompileShader(blobPtr->psBlob.GetAddressOf(), filename, entryPoint, "ps_5_0"); }

    static void CreateRootParameter(std::vector<D3D12_ROOT_PARAMETER>& rootParams ,RootParameterStructure_t rootParameterStructure);
    static D3D12_ROOT_SIGNATURE_DESC CreateRootSignatureDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, size_t patternNum, const std::vector<D3D12_ROOT_PARAMETER>& rootParamsRef);
    static void VSerializeRootSignature(ID3DBlob** rootSigBlobDPtr, D3D12_ROOT_SIGNATURE_DESC* rootSignatureDescPtr);
    static void VCreateRootSignature(ID3DBlob* rootSigBlobPtr, ID3D12RootSignature** rootSignatureDPtr);

    static D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePipelineDesc(Blob_t* blobsPtr, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, const PSO_t& psoRef, D3D12_CULL_MODE cullmode, BlendMode mode = BlendMode::NONE);
    static void VCreatePSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineDescPtr, ID3D12PipelineState** psoDPtr);
private:
    static void SetSamplerDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef,size_t patternNum);
    static void SetCompileShader(ID3DBlob** dp_blob, const std::string& filename, const std::string& entryPoint, const std::string& target);
    static void SetBlend(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipelineDesc,BlendMode mode = BlendMode::NONE);
};

class GraphicsPipeline : public HelperGraphicPipeline
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    // �֐�
    void Create(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, size_t patternNum, Blob_t* blobsPtr, size_t  rootParamsCBNum, D3D12_CULL_MODE cullmode, BlendMode mode = BlendMode::NONE);

private:
    // �ϐ�
    PSO_t pipelineStateObject_{};

public:
    // setter�Egetter
    inline ID3D12RootSignature* GetRootSignaturePtr(void) { pipelineStateObject_.rootSignature.Get(); }
    inline ID3D12PipelineState* GetPipelineStatePtr(void) { pipelineStateObject_.pipelineState.Get(); }
    inline PSO_t* GetAddresOf(void) { return &pipelineStateObject_; }
};

class PSOManager : public GraphicsPipeline
{
private:
    // ��`
    enum class Shader // �ǂ��܂ł��̖ڈ�
    {
        VS,
        GS,
        PS,
    };

public:
    // �֐�
    static PSOManager* GetInstance(void);

    void Create();

private:
    void CreateAllBlendType(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, size_t patternNum, Blob_t* blobsPtr, size_t  rootParamsCBNum, D3D12_CULL_MODE cullmode, const std::string& mapKey);

    // �ϐ�
    std::map<std::string, std::array<GraphicsPipeline, static_cast<size_t>(HelperGraphicPipeline::BlendMode::MAX_ARRAY)>> pipelineStateObjects_{};

public:
    // setter�Egetter
    inline std::map<std::string, std::array<GraphicsPipeline, static_cast<size_t>(HelperGraphicPipeline::BlendMode::MAX_ARRAY)>>* GetPipelineStateObjectsMap(void) { return &pipelineStateObjects_; }
    inline GraphicsPipeline::PSO_t* GetPSO(const std::string key, HelperGraphicPipeline::BlendMode mode = HelperGraphicPipeline::BlendMode::NONE) { return pipelineStateObjects_[key][static_cast<size_t>(mode)].GetAddresOf(); }

private:
    // �V���O���g��
    PSOManager(void) {};
    ~PSOManager(void) {};
    PSOManager(const PSOManager&) = delete;
    PSOManager& operator=(const PSOManager&) = delete;
};