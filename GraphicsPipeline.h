#pragma once
#include <wrl.h>
#include <string>
#include <array>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

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

protected:
    struct Blob_t
    {
        // 頂点シェーダオブジェクト
        ComPtr<ID3DBlob> vsBlob{ nullptr };
        // ジオメトリシェーダオブジェクト
        ComPtr<ID3DBlob> gsBlob{ nullptr };
        // ピクセルシェーダオブジェクト
        ComPtr<ID3DBlob> psBlob{ nullptr };
    };

    struct Pipeline_t
    {
        // ルートシグネチャ
        ComPtr<ID3D12RootSignature> rootSignature{};
        // パイプランステートの生成
        ComPtr<ID3D12PipelineState> pipelineState{};
    };

    static void Pipeline2d(Pipeline_t& pipeline, BlendMode mode = BlendMode::NONE);
    static void Pipeline3d(Pipeline_t& pipeline, BlendMode mode = BlendMode::NONE);

private:
    static void SetCompileShader(ID3DBlob** dp_blob, const std::string& filename, const std::string& entryPoint, const std::string& target);
};

class GraphicsPipeline : public HelperGraphicPipeline
{
public:

    // 静的関数
    static GraphicsPipeline* GetInstance(void);

    // 関数
    void Initialize(void);
    const Pipeline_t& GetPipeline2d(BlendMode mode) const;
    const Pipeline_t& GetPipeline3d(BlendMode mode) const;

private:

    std::array<Pipeline_t, static_cast<uint32_t>(BlendMode::MAX_ARRAY)> pipelineProducts2d_;
    std::array<Pipeline_t, static_cast<uint32_t>(BlendMode::MAX_ARRAY)> pipelineProducts3d_;

    // シングルトン
    GraphicsPipeline(void) {};
    ~GraphicsPipeline(void) {};
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
};