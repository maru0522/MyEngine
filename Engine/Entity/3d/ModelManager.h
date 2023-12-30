#pragma once
#include <map>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>
#pragma warning(push)
#pragma warning(disable:4820)
#include <d3d12.h>
#pragma warning(pop)

#include "TextureManager.h"
#include "VertexBuffer.h"
#include <filesystem>
#include "IndexBuffer.h"
#include "ConstBuffer.h"
#include "PSOManager.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Mesh.h"
/**
 * @file ModelManager.h
 * @brief モデルについての情報を含むModel_t構造体や、ModelManager等が宣言されたファイル
 */

struct Model_t final
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::filesystem::path;

public:
    struct Material_t
    {
        std::string name{};           // マテリアルの名前
        Vector3 ambient{};            // ambient（環境）影響度
        Vector3 diffuse{};            // diffuse（広汎）影響度
        Vector3 specular{};           // specular（鏡面反射）影響度
        float_t alpha{};              // アルファ
        fsPath texKey{};              // テクスチャファイル名

        Material_t() :
            ambient(0.3f, 0.3f, 0.3f),
            diffuse(0.0f, 0.0f, 0.0f),
            specular(0.0f, 0.0f, 0.0f),
            alpha(1.0f) {}
    };

    struct CBMaterial_t // 定数バッファ用構造体
    {
        Vector3 ambient;    // ambient
        float_t pad1;
        Vector3 diffuse;    // diffuse
        float_t pad2;
        Vector3 specular;   // specular
        float_t alpha;      // alpha
    };

    // 関数
    void UpdateCB(void);

    // 変数
    Mesh* meshPtr_;
    Material_t material_;

    bool isSmoothing_{};
    ConstBuffer<CBMaterial_t> cbMaterial_;
};


class ModelManager
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::filesystem::path;
    using Material_t = Model_t::Material_t;

public:
    // 関数
    ModelManager(TextureManager* texMPtr) : texMPtr_(texMPtr) {}
    void LoadOBJ(const fsPath& path, bool smoothing);

private:
    void LoadMaterial(Model_t& model,const fsPath& path);

    // 変数
    // << モデルの頂点データ保管コンテナ >>
    // << モデルの保管コンテナ >>
    std::map<fsPath, Mesh> meshes_;
    std::map<fsPath, Model_t> models_;

    TextureManager* texMPtr_;

public:
    // setter・getter
    Model_t GetModel(const fsPath& path);
    //const Model_t* GetModelPtrByNickname(const std::string nickname);
};
