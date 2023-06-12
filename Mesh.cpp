#include "Mesh.h"

void Mesh::CalcSmoothedVertNormals(std::vector<VertexPosNormalUv_t>& vertices)
{
    for (auto& it : smoothData_) {
        // �e�ʗp�̋��ʒ��_�R���N�V����
        std::vector<uint16_t>& v = it.second;
        // �S���_�̖@���𕽋ς���
        Vector3 normal{};
        for (uint16_t idx : v) {
            normal += {vertices[idx].normal_.x, vertices[idx].normal_.y, vertices[idx].normal_.z };
        }
        normal = (normal / (float)v.size()).normalize();
        // ���ʖ@�����g�p���邷�ׂĂ�
        for (uint16_t idx : v) {
            vertices[idx].normal_ = { normal.x,normal.y,normal.z };
        }
    }
}
