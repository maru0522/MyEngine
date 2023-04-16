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
    // 定義
    using fsPath = std::experimental::filesystem::path;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData_t // 定数バッファ用データ構造体
    {
        DirectX::XMMATRIX mat_;  // 3D変換行列
        DirectX::XMFLOAT4 color_; // 色（RGBA）
    };

    struct VertexPosUv_t
    {
        Vector3 pos; // xyz座標
        Vector2 uv; // uv座標
    };

public:
    // 静的関数
    static void PreDraw(void);

    // 関数
    Sprite();
    void Update(void);
    void Draw(void);

private:
    // 変数
    Sprite* parent_;

    Vector2 position_;
    Vector2 scale_;
    Vector2 size_;
    float rotation_;

    std::unique_ptr<ConstBuffer<CBData_t>> cb_; // 定数バッファ

    std::unique_ptr<VertexBuffer<VertexPosUv_t>> vertexBuffer_; // 頂点バッファ

    D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandleCopy_;

public:
    // setter・getter
    inline const Vector2& GetPosition(void) { return position_; }
    inline float GetRotation(void) { return rotation_; }

    inline void SetPosition(const Vector2& posotion) { position_ = posotion; }
    inline void SetRotation(float rotation) { rotation_ = rotation; }

};

