#pragma once
#include "VertexBuffer.h"
#include <unordered_map>
#include "IndexBuffer.h"
#include <filesystem>
#include "Vector3.h"
#include "Vector2.h"
/**
 * @file Mesh.h
 * @brief モデル用の頂点や、インデックスをまとめて保持するクラスが宣言されたファイル
 */

class Mesh
{
private:
    // 定義
    using fsPath = std::filesystem::path;

public:
    struct VertexPosNormalUv_t // 頂点バッファ用"データ"構造体
    {
        Vector3 pos;    // xyz座標
        Vector3 normal; // 法線ベクトル
        Vector2 uv;     // uv座標
    };

    // 関数
    Mesh(void) = default;
    Mesh(const fsPath& path) : path_(path) {}

    inline void CreateIB(const std::vector<uint16_t>& indices) { indexBuffer_.Create(indices); }
    inline void CreateVB(const std::vector<VertexPosNormalUv_t>& vertices) { vertexBuffer_.Create(vertices); }

    void CalcSmoothedVertNormals(std::vector<VertexPosNormalUv_t>& vertices);
    inline void AddSmoothData(uint16_t indexPos, uint16_t indexVert) { smoothData_[indexPos].emplace_back(indexVert); }

    // setter・getter
    inline void SetPath(const fsPath& path) { path_ = path; }

    inline fsPath& GetPath(void) { return path_; }
    inline IndexBuffer* GetIBPtr(void) { return &indexBuffer_; }
    inline VertexBuffer<VertexPosNormalUv_t>* GetVBPtr(void) { return &vertexBuffer_; }

private:
    // 変数
    fsPath path_{}; // KEY

    IndexBuffer indexBuffer_{};
    VertexBuffer<VertexPosNormalUv_t> vertexBuffer_{};
    std::unordered_map<uint16_t, std::vector<uint16_t>> smoothData_{};
};
