#include "Mesh.h"

void Mesh::CalcSmoothedVertNormals(std::vector<VertexPosNormalUv_t>& vertices)
{
    for (auto& it : smoothData_) {
        // �e�ʗp�̋��ʒ��_�R���N�V����
        std::vector<uint16_t>& v = it.second;
        // �S���_�̖@���𕽋ς���
        Vector3 normal{};
        for (uint16_t idx : v) {
            normal += {vertices[idx].normal.x, vertices[idx].normal.y, vertices[idx].normal.z };
        }
        normal = (normal / (float)v.size()).Normalize();
        // ���ʖ@�����g�p���邷�ׂĂ�
        for (uint16_t idx : v) {
            vertices[idx].normal = { normal.x,normal.y,normal.z };
        }
    }
}
