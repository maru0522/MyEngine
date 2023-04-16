#pragma once
#include <array>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#if _MSC_VER > 1922 && !defined(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "ConstBuffer.h"
#include "WndAPI.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "ConstBuffer.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4.h"

class Sprite
{
private:
    // ��`
    using fsPath = std::experimental::filesystem::path;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData_t // �萔�o�b�t�@�p�f�[�^�\����
    {
        DirectX::XMMATRIX mat_;  // 3D�ϊ��s��
        DirectX::XMFLOAT4 color_; // �F�iRGBA�j
    };

    struct VertexPosUv_t
    {
        Vector3 pos; // xyz���W
        Vector2 uv; // uv���W
    };

public:
    // �ÓI�֐�
    static void PreDraw(void);

    // �֐�
    Sprite();
    void Update(void);
    void Draw(void);

private:
    // �ϐ�
    Sprite* parent_;

    Vector2 position_;
    Vector2 scale_;
    Vector2 size_;
    float rotation_;

    std::unique_ptr<ConstBuffer<CBData_t>> cb_; // �萔�o�b�t�@

    std::unique_ptr<VertexBuffer<VertexPosUv_t>> vertexBuffer_; // ���_�o�b�t�@

    D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandleCopy_;

public:
    // setter�Egetter
    inline const Vector2& GetPosition(void) { return position_; }
    inline float GetRotation(void) { return rotation_; }

    inline void SetPosition(const Vector2& posotion) { position_ = posotion; }
    inline void SetRotation(float rotation) { rotation_ = rotation; }

};

