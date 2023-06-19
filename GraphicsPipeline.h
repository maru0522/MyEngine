#pragma once
#include <wrl.h>
#include <string>
#include <array>
#include <d3d12.h>
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
    struct Blob_t
    {
        // ���_�V�F�[�_�I�u�W�F�N�g
        ComPtr<ID3DBlob> vsBlob{ nullptr };
        // �W�I���g���V�F�[�_�I�u�W�F�N�g
        ComPtr<ID3DBlob> gsBlob{ nullptr };
        // �s�N�Z���V�F�[�_�I�u�W�F�N�g
        ComPtr<ID3DBlob> psBlob{ nullptr };
    };

    struct Pipeline_t
    {
        // ���[�g�V�O�l�`��
        ComPtr<ID3D12RootSignature> rootSignature{};
        // �p�C�v�����X�e�[�g�̐���
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

    // �ÓI�֐�
    static GraphicsPipeline* GetInstance(void);

    // �֐�
    void Initialize(void);
    const Pipeline_t& GetPipeline2d(BlendMode mode) const;
    const Pipeline_t& GetPipeline3d(BlendMode mode) const;

private:

    std::array<Pipeline_t, static_cast<uint32_t>(BlendMode::MAX_ARRAY)> pipelineProducts2d_;
    std::array<Pipeline_t, static_cast<uint32_t>(BlendMode::MAX_ARRAY)> pipelineProducts3d_;

    // �V���O���g��
    GraphicsPipeline(void) {};
    ~GraphicsPipeline(void) {};
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
};