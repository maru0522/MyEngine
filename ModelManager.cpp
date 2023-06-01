#include "ModelManager.h"
#include "Util.h"
#include <fstream>
#include <sstream>

using namespace Util;
using VertexPosNormalUv_t = Mesh::VertexPosNormalUv_t;

void ModelManager::LoadOBJ(const fsPath& path)
{
    // ���ɓǂݍ���model�f�[�^�Ƃ̏d���m�F�B
    if (models_.count(path)) {
        Util::Log::PrintOutputWindow("A .obj file with the same name was loaded. Some models may not have been loaded.");
        return;
    }

    // �z��p�̈ꎞModel
    Model_t tempModel;
    // �z��p�̈ꎞMesh
    Mesh tempMesh;
    tempMesh.SetPath(path);

    // ���_�ƃC���f�b�N�X���
    std::ifstream ifs;
    ifs.open(path);
    assert(!ifs.fail());

    std::vector<Vector3> positions{}; // ���_
    std::vector<Vector3> normals{};   // �@��
    std::vector<Vector2> texcoords{}; // uv

    std::vector<VertexPosNormalUv_t> vertices{};
    std::vector<unsigned short> indices{};

    std::string line;
    while (std::getline(ifs, line))
    {
        std::istringstream line_iss{ line };

        std::string key;
        std::getline(line_iss, key, ' ');

        if (key == "mtllib")
        {
            std::string mtlFileName;
            line_iss >> mtlFileName;

            LoadMaterial(tempModel, path);
        }

        // ���_
        if (key == "v")
        {
            Vector3 pos;
            line_iss >> pos.x;
            line_iss >> pos.y;
            line_iss >> pos.z;

            positions.emplace_back(pos);
        }

        // �e�N�X�`��
        if (key == "vt")
        {
            Vector2 texcoord{};
            line_iss >> texcoord.x;
            line_iss >> texcoord.y;

            texcoord.y = 1.0f - texcoord.y; // V�������]
            texcoords.emplace_back(texcoord);
        }

        // �@��
        if (key == "vn")
        {
            Vector3 normal{};
            line_iss >> normal.x;
            line_iss >> normal.y;
            line_iss >> normal.z;

            normals.emplace_back(normal);
        }

        if (key == "f")
        {
            std::string index_str;
            while (std::getline(line_iss, index_str, ' '))
            {

                std::istringstream index_iss{ index_str };
                unsigned short indexPos{};
                unsigned short indexNormal{};
                unsigned short indexTexcoord{};

                index_iss >> indexPos;
                index_iss.seekg(1, std::ios_base::cur); // �X���b�V�����΂�
                index_iss >> indexTexcoord;
                index_iss.seekg(1, std::ios_base::cur); // �X���b�V�����΂�
                index_iss >> indexNormal;

                VertexPosNormalUv_t vertex{};
                vertex.pos_ = positions[indexPos - 1];
                vertex.normal_ = normals[indexNormal - 1];
                vertex.uv_ = texcoords[indexTexcoord - 1];
                // ���_�f�[�^�̒ǉ�
                vertices.emplace_back(vertex);
                // ���_�C���f�b�N�X�f�[�^�̒ǉ�
                indices.emplace_back((uint16_t)indices.size());
                //-> indices.size()��emplace_back()���邱�Ƃɂ���āA�ǉ����ꂽ���_�f�[�^�̗v�f���ɍ��킹���C���f�b�N�X���ǉ�����Ă���
            }
        }
    }
    // ���_�o�b�t�@
    tempMesh.CreateVB(vertices);
    // �C���f�b�N�X�o�b�t�@
    tempMesh.CreateIB(indices);

    // �z��ɒǉ�
    meshes_.emplace(path, tempMesh);

    // ���_�f�[�^mesh�̕ۊǃA�h���X����������B
    tempModel.meshPtr_ = &meshes_[path];
    // �z��ɒǉ� ���萔�o�b�t�@������Object3D�̃R���X�g���N�^�ōs���B
    models_.emplace(path, tempModel);
}

void ModelManager::LoadMaterial(Model_t& model, const fsPath& path)
{
    const std::string mtlname{ path.stem().string() + ".mtl" };

    // �t�@�C���X�g���[��
    std::ifstream file;
    // �}�e���A���t�@�C�����J��
    file.open(path.parent_path().string() + "/" + mtlname);
    // �t�@�C���I�[�v�����s���`�F�b�N
    assert(!file.fail());

    // 1�s���ǂݍ���
    std::string line;
    while (std::getline(file, line)) {
        // 1�s���̕�������X�g���[���ɕϊ�
        std::istringstream line_iss(line);

        // ���p�X�y�[�X��؂�ōs�̐擪��������擾
        std::string key;
        std::getline(line_iss, key, ' ');

        // �擪�̃^�u�����͖�������
        if (key[0] == '\t') {
            key.erase(key.begin()); // �擪�̕������폜
        }

        // �擪������newmtl�Ȃ�}�e���A����
        if (key == "newmtl") {
            // �}�e���A�����ǂݍ���
            line_iss >> model.material_.name_;
        }

        // �擪������Ka�Ȃ�A���r�G���g�F
        if (key == "Ka") {
            line_iss >> model.material_.ambient_.x;
            line_iss >> model.material_.ambient_.y;
            line_iss >> model.material_.ambient_.z;
        }

        // �擪������Ȃ�Kd�Ȃ�f�B�t���[�Y�F
        if (key == "kd") {
            line_iss >> model.material_.diffuse_.x;
            line_iss >> model.material_.diffuse_.y;
            line_iss >> model.material_.diffuse_.z;
        }

        // �擪������Ks�Ȃ�X�y�L�����[�F
        if (key == "Ks") {
            line_iss >> model.material_.specular_.x;
            line_iss >> model.material_.specular_.y;
            line_iss >> model.material_.specular_.z;
        }

        // �擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
        if (key == "map_Kd") {
            std::string tmpName;

            // �e�N�X�`���̃t�@�C�����ǂݍ���
            line_iss >> tmpName;
            model.material_.texKey_ = path.parent_path() / tmpName;

            // �e�N�X�`���ǂݍ���
            texMPtr_->Load(model.material_.texKey_);
        }
    }
}

Model_t ModelManager::GetModel(const fsPath& path)
{
    try
    {
        return models_[path];
    }
    catch (const std::exception&)
    {
        // �G���[���O
        Util::Log::PrintOutputWindow("[ModelManager] : Couldn't find a 3Dmodel corresponding to the argument path(" + path.string() + "), so replaced it with \"cube\". ");
        // �w�肳�ꂽ���f����������Ȃ������ꍇcube�ɒu�������ĕԋp����
        return models_["Resources/cube/cube.obj"];
    }
}

void Model_t::UpdateCB(void)
{
    // �萔�o�b�t�@�̃}�e���A���̒l���X�V����
    cbMaterial_.GetConstBuffMap()->ambient = material_.ambient_;
    cbMaterial_.GetConstBuffMap()->diffuse = material_.diffuse_;
    cbMaterial_.GetConstBuffMap()->specular = material_.specular_;
    cbMaterial_.GetConstBuffMap()->alpha = material_.alpha_;
}
