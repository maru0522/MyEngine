#pragma once
#include <map>
#include <wrl.h>
#include <array>
#include <string>
#pragma warning(push)
#pragma warning(disable:4820)
#include <d3d12.h>
#pragma warning(pop)
#include <d3dx12.h>
/**
 * @file PSOManager.h
 * @brief GraphicsPipelineを管理するクラスやHelperクラス等が書かれたファイル
 */

class HelperGraphicPipeline
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    enum class BlendMode
    {
        // 無し
        NONE,
        // 半透明合成
        ALPHA,
        // 加算合成
        ADDITIVE,
        // 減算合成
        SUBTRACTIVE,
        // 乗算合成
        MULTIPLICATION,
        // 色反転
        INVERSION,

        MAX_ARRAY,
    };

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

protected:
    struct PSO_t
    {
        // ルートシグネチャ
        ComPtr<ID3D12RootSignature> rootSignature{};
        // パイプランステートの生成
        ComPtr<ID3D12PipelineState> pipelineState{};
    };

    struct Blob_t
    {
        // 頂点シェーダオブジェクト
        ComPtr<ID3DBlob> vsBlob{ nullptr };
        // ジオメトリシェーダオブジェクト
        ComPtr<ID3DBlob> gsBlob{ nullptr };
        // ピクセルシェーダオブジェクト
        ComPtr<ID3DBlob> psBlob{ nullptr };
    };

    struct RootParameterStructure_t
    {
        size_t descriptorRangeNum{};
        size_t rootParamsCBNum{};
    };

    static void VSetCompileShader(ID3DBlob** dp_blob, const std::string& filename, const std::string& entryPoint, const std::string& target);

    static std::vector<D3D12_ROOT_PARAMETER> CreateRootParameter(RootParameterStructure_t rootParameterStructure);
    static D3D12_STATIC_SAMPLER_DESC CreateSamplerDesc(SamplerType type);
    static D3D12_ROOT_SIGNATURE_DESC CreateRootSignatureDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, const std::vector<CD3DX12_ROOT_PARAMETER>& cRootParamsRef);
    static void VSerializeRootSignature(ID3DBlob** rootSigBlobDPtr, D3D12_ROOT_SIGNATURE_DESC* rootSignatureDescPtr);
    static void VCreateRootSignature(ID3DBlob* rootSigBlobPtr, ID3D12RootSignature** rootSignatureDPtr);

    static D3D12_GRAPHICS_PIPELINE_STATE_DESC CreatePipelineDesc(Blob_t* blobsPtr, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, const PSO_t& psoRef, D3D12_CULL_MODE cullMode, BlendMode blendMode,bool isDepthEnable);
    static void VCreatePSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineDescPtr, ID3D12PipelineState** psoDPtr);
private:
    static void SetSamplerDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef,size_t patternNum);
    static void SetBlend(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipelineDesc,BlendMode mode = BlendMode::NONE);
};

class IGraphicsPipeline : public HelperGraphicPipeline
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

protected:
    // pipelineStateObject_を生成する際の設定
    struct IGraphicsPipelineStructure_t
    {
        SamplerType samplerType{};
        D3D12_CULL_MODE cullMode{};
        RootParameterStructure_t rps{};
        std::unique_ptr<Blob_t> blobs{};
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts{};
        bool isDepthEnable{ true };

        IGraphicsPipelineStructure_t(void)
        {
            blobs = std::make_unique<Blob_t>();
        }
    };

public:
    // 関数
    IGraphicsPipeline(void) = default;
    void Create(IGraphicsPipelineStructure_t* igpsPtr, BlendMode blendMode);

private:
    // 変数
    PSO_t pipelineStateObject_{};

public:
    // setter・getter
    inline ID3D12RootSignature* GetRootSignaturePtr(void) { pipelineStateObject_.rootSignature.Get(); }
    inline ID3D12PipelineState* GetPipelineStatePtr(void) { pipelineStateObject_.pipelineState.Get(); }
    inline PSO_t* GetPipelineStateObjectPtr(void) { return &pipelineStateObject_; }
};

class PSOManager : public IGraphicsPipeline
{
private:
    // 定義
    struct GraphicsPipelineBlend_t
    {
        std::array<IGraphicsPipeline, static_cast<size_t>(HelperGraphicPipeline::BlendMode::MAX_ARRAY)> blends;
        IGraphicsPipelineStructure_t igps{};

        void Create(void);
        inline void SetCullMode(D3D12_CULL_MODE cullmode) { igps.cullMode = cullmode; }
        inline void SetSamplerType(SamplerType samplerType) { igps.samplerType = samplerType; }
        inline void AddInputLayout(D3D12_INPUT_ELEMENT_DESC inputLayout) { igps.inputLayouts.emplace_back(inputLayout); }
        void CompileShader(ShaderType shaderType, const std::string& filename, const std::string& entryPoint);
        void SetRootParameterStructure(size_t descriptorRangeNum, size_t rootParamsCBNum);
        void SetDepthEnable(bool arg_isDepthEnable) { igps.isDepthEnable = arg_isDepthEnable; }
    };

public:
    // 関数
    static PSOManager* GetInstance(void);

    void Create();

private:
    inline void AddPipeline(const std::string& key) { pipelineStateObjects_.emplace(key, GraphicsPipelineBlend_t()); }
    inline void SetComplete(const std::string& key) { pipelineStateObjects_[key].Create(); }

    // 変数
    std::map<std::string, GraphicsPipelineBlend_t> pipelineStateObjects_{};

public:
    // setter・getter
    inline std::map<std::string, GraphicsPipelineBlend_t>* GetPSOsMap(void) { return &pipelineStateObjects_; }
    inline GraphicsPipelineBlend_t* GetPSOBlendPtr(const std::string key) { return &pipelineStateObjects_[key]; }
    inline IGraphicsPipeline::PSO_t* GetPSOPtr(const std::string key, HelperGraphicPipeline::BlendMode blendMode = HelperGraphicPipeline::BlendMode::NONE) { return pipelineStateObjects_[key].blends[static_cast<size_t>(blendMode)].GetPipelineStateObjectPtr(); }

private:
    // シングルトン
    PSOManager(void) {};
    ~PSOManager(void) {};
    PSOManager(const PSOManager&) = delete;
    PSOManager& operator=(const PSOManager&) = delete;
};
