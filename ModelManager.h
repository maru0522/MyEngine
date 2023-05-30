#pragma once
#include <map>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "GraphicsPipeline.h"
#include "TextureManager.h"
#include "VertexBuffer.h"
#include "FileSystem.h"
#include "IndexBuffer.h"
#include "ConstBuffer.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Mesh.h"

struct Model_t
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

public:
    struct Material_t
    {
        std::string name_{};           // マテリアルの名前
        DirectX::XMFLOAT3 ambient_{};  // ambient（環境）影響度
        DirectX::XMFLOAT3 diffuse_{};  // diffuse（広汎）影響度
        DirectX::XMFLOAT3 specular_{}; // specular（鏡面反射）影響度
        float_t alpha_{};              // アルファ
        fsPath texKey_{};              // テクスチャファイル名

        Material_t() :
            ambient_(0.3f, 0.3f, 0.3f),
            diffuse_(0.0f, 0.0f, 0.0f),
            specular_(0.0f, 0.0f, 0.0f),
            alpha_(1.0f) {}
    };

    struct CBMaterial_t // 定数バッファ用構造体
    {
        DirectX::XMFLOAT3 ambient;  // ambient
        float_t pad1; // padding
        DirectX::XMFLOAT3 diffuse;  // diffuse
        float_t pad2; // padding
        DirectX::XMFLOAT3 specular; // specular
        float_t alpha;              // alpha
    };

    // 関数
    void UpdateCB(void);

    // 変数
    Mesh mesh_;
    Material_t material_;

    ConstBuffer<CBMaterial_t> cbMaterial_;
};


class ModelManager
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

public:
    // 関数
    ModelManager(TextureManager* texMPtr) : texMPtr_(texMPtr) {}
    void LoadOBJ(const fsPath& path);

private:
    void LoadMaterial(Model_t& model, const fsPath& path);

    // 変数
    std::map<fsPath, Model_t> models_;

    TextureManager* texMPtr_;

public:
    // setter・getter
    Model_t GetModel(const fsPath& path) const;
    const Model_t* GetModelPtr(const fsPath& path) const;
    //const Model_t* GetModelPtrByNickname(const std::string nickname);
};
