#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "FileSystem.h"
#include "Vector3.h"
#include "Vector2.h"

class Mesh
{
private:
    // ��`
    using fsPath = std::experimental::filesystem::path;

public:
    struct VertexPosNormalUv_t // ���_�o�b�t�@�p�\����
    {
        Vector3 pos_;    // xyz���W
        Vector3 normal_; // �@���x�N�g��
        Vector2 uv_;     // uv���W
    };

    // �֐�
    Mesh(void) = default;
    Mesh(const fsPath& path) : path_(path) {}

    inline void CreateIB(const std::vector<unsigned short>& indices) { indexBuffer_ = std::make_unique<IndexBuffer>(indices); }
    inline void CreateVB(const std::vector<VertexPosNormalUv_t>& vertices) { vertexBuffer_ = std::make_unique<VertexBuffer<VertexPosNormalUv_t>>(vertices); }

    // setter�Egetter
    inline void SetPath(const fsPath& path) { path_ = path; }

    inline const fsPath& GetPath(void) { return path_; }
    inline const IndexBuffer* GetIBPtr(void) { return indexBuffer_.get(); }
    inline const VertexBuffer<VertexPosNormalUv_t>* GetVBPtr(void) { return vertexBuffer_.get(); }

private:
    // �ϐ�
    fsPath path_{}; // KEY

    std::unique_ptr<IndexBuffer> indexBuffer_{};
    std::unique_ptr<VertexBuffer<VertexPosNormalUv_t>> vertexBuffer_{};
};

