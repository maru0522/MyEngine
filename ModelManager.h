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
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

public:
    struct Material_t
    {
        std::string name_{};           // �}�e���A���̖��O
        DirectX::XMFLOAT3 ambient_{};  // ambient�i���j�e���x
        DirectX::XMFLOAT3 diffuse_{};  // diffuse�i�L�āj�e���x
        DirectX::XMFLOAT3 specular_{}; // specular�i���ʔ��ˁj�e���x
        float_t alpha_{};              // �A���t�@
        fsPath texKey_{};              // �e�N�X�`���t�@�C����

        Material_t() :
            ambient_(0.3f, 0.3f, 0.3f),
            diffuse_(0.0f, 0.0f, 0.0f),
            specular_(0.0f, 0.0f, 0.0f),
            alpha_(1.0f) {}
    };

    struct CBMaterial_t // �萔�o�b�t�@�p�\����
    {
        DirectX::XMFLOAT3 ambient;  // ambient
        float_t pad1; // padding
        DirectX::XMFLOAT3 diffuse;  // diffuse
        float_t pad2; // padding
        DirectX::XMFLOAT3 specular; // specular
        float_t alpha;              // alpha
    };

    // �֐�
    void UpdateCB(void);

    // �ϐ�
    Mesh mesh_;
    Material_t material_;

    ConstBuffer<CBMaterial_t> cbMaterial_;
};


class ModelManager
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

public:
    // �֐�
    ModelManager(TextureManager* texMPtr) : texMPtr_(texMPtr) {}
    void LoadOBJ(const fsPath& path);

private:
    void LoadMaterial(Model_t& model, const fsPath& path);

    // �ϐ�
    std::map<fsPath, Model_t> models_;

    TextureManager* texMPtr_;

public:
    // setter�Egetter
    Model_t GetModel(const fsPath& path) const;
    const Model_t* GetModelPtr(const fsPath& path) const;
    //const Model_t* GetModelPtrByNickname(const std::string nickname);
};
