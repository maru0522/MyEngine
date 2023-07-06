#include "Mesh.h"

void Mesh::CalcSmoothedVertNormals(std::vector<VertexPosNormalUv_t>& vertices)
{
    for (auto& it : smoothData_) {
        // 各面用の共通頂点コレクション
        std::vector<uint16_t>& v = it.second;
        // 全頂点の法線を平均する
        Vector3 normal{};
        for (uint16_t idx : v) {
            normal += {vertices[idx].normal.x, vertices[idx].normal.y, vertices[idx].normal.z };
        }
        normal = (normal / (float)v.size()).Normalize();
        // 共通法線を使用するすべての
        for (uint16_t idx : v) {
            vertices[idx].normal = { normal.x,normal.y,normal.z };
        }
    }
}
