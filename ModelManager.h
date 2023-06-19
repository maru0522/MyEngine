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
        std::string name{};           // �}�e���A���̖��O
        Vector3 ambient{};            // ambient�i���j�e���x
        Vector3 diffuse{};            // diffuse�i�L�āj�e���x
        Vector3 specular{};           // specular�i���ʔ��ˁj�e���x
        float_t alpha{};              // �A���t�@
        fsPath texKey{};              // �e�N�X�`���t�@�C����

        Material_t() :
            ambient(0.3f, 0.3f, 0.3f),
            diffuse(0.0f, 0.0f, 0.0f),
            specular(0.0f, 0.0f, 0.0f),
            alpha(1.0f) {}
    };

    struct CBMaterial_t // �萔�o�b�t�@�p�\����
    {
        Vector3 ambient;    // ambient
        float_t pad1;
        Vector3 diffuse;    // diffuse
        float_t pad2;
        Vector3 specular;   // specular
        float_t alpha;      // alpha
    };

    // �֐�
    void UpdateCB(void);

    // �ϐ�
    Mesh* meshPtr_;
    Material_t material_;

    bool isSmoothing_;
    ConstBuffer<CBMaterial_t> cbMaterial_;
};


class ModelManager
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;
    using Material_t = Model_t::Material_t;

public:
    // �֐�
    ModelManager(TextureManager* texMPtr) : texMPtr_(texMPtr) {}
    void LoadOBJ(const fsPath& path, bool smoothing);

private:
    void LoadMaterial(Model_t& model,const fsPath& path);

    // �ϐ�
    // << ���f���̒��_�f�[�^�ۊǃR���e�i >>
    // << ���f���̕ۊǃR���e�i >>
    std::map<fsPath, Mesh> meshes_;
    std::map<fsPath, Model_t> models_;

    TextureManager* texMPtr_;

public:
    // setter�Egetter
    Model_t GetModel(const fsPath& path);
    //const Model_t* GetModelPtrByNickname(const std::string nickname);
};
