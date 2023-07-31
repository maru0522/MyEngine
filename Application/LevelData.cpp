#include "LevelData.h"
#include "MathUtil.h"
#include <fstream>
#include <cassert>

std::unique_ptr<LevelData> LevelData::Load(const std::string& path)
{
    std::ifstream ifs;
    ifs.open(path);
    assert(!ifs.fail());

    // json������̃f�[�^
    nlohmann::json deserialized;

    // ��
    ifs >> deserialized;

    // ���������x���f�[�^�t�@�C�����`�F�b�N
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    // "name"�𕶎���Ƃ��Ď擾
    std::string name = deserialized["name"].get<std::string>();
    // ���������x���f�[�^�t�@�C�����`�F�b�N
    assert(name.compare("scene") == 0);

    // ���x���f�[�^�i�[�p�C���X�^���X����
    std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

    // objects�̑S�I�u�W�F�N�g�𑖍�
    for (nlohmann::json& object : deserialized["objects"]) {
        ScanRecursive(object, levelData.get());

        // �ċN�ďo�Ŏ}�𑖍�����
        for (nlohmann::json& child : object["children"])
        {
            ScanRecursive(child, levelData.get());
        }
    }

    return std::move(levelData);
}

void LevelData::ScanRecursive(nlohmann::json& jsonObject, LevelData* levelDataPtr)
{
    assert(jsonObject.contains("type"));

    // ��ʂ��擾
    std::string type = jsonObject["type"].get<std::string>();

    // MESH
    if (type.compare("MESH") == 0) {
        // �v�f�ǉ�
        levelDataPtr->objects_.emplace_back(LevelData::ObjectData_t{});
        // ���ǉ������v�f�̎Q�Ƃ𓾂�
        LevelData::ObjectData_t& objectData = levelDataPtr->objects_.back();
        objectData.type = type;

        if (jsonObject.contains("file_name")) {
            // �t�@�C����
            objectData.file_name = jsonObject["file_name"];
        }

        objectData.name = jsonObject["name"].get<std::string>();

        // �g�����X�t�H�[���̃p�����[�^�ǂݍ���
        nlohmann::json& transform = jsonObject["transform"];

        // ���s�ړ�
        objectData.trans.x = (float)transform["translation"][0];
        objectData.trans.y = (float)transform["translation"][2];
        objectData.trans.z = (float)transform["translation"][1];

        // ��]�p
        objectData.rot.x = Math::Function::ToRadian(-(float)transform["rotation"][0]);
        objectData.rot.y = Math::Function::ToRadian(-(float)transform["rotation"][2]);
        objectData.rot.z = Math::Function::ToRadian((float)transform["rotation"][1]);

        // �X�P�[�����O
        objectData.scale.x = (float)transform["scaling"][0];
        objectData.scale.y = (float)transform["scaling"][2];
        objectData.scale.z = (float)transform["scaling"][1];

        if (jsonObject.contains("invisible_flag")) objectData.isInvisible = jsonObject["invisible_flag"].get<uint32_t>();
        else objectData.isInvisible = false;
    }

    // LIGHT
    if (type.compare("LIGHT") == 0) {
        // �v�f�ǉ�
        levelDataPtr->objects_.emplace_back(LevelData::ObjectData_t{});
        // ���ǉ������v�f�̎Q�Ƃ𓾂�
        LevelData::ObjectData_t& objectData = levelDataPtr->objects_.back();
        objectData.type = type;

        if (jsonObject.contains("file_name")) {
            // �t�@�C����
            objectData.file_name = jsonObject["file_name"];
        }

        objectData.name = jsonObject["name"].get<std::string>();

        // �g�����X�t�H�[���̃p�����[�^�ǂݍ���
        nlohmann::json& transform = jsonObject["transform"];

        // ���s�ړ�
        objectData.trans.x = (float)transform["translation"][0];
        objectData.trans.y = (float)transform["translation"][2];
        objectData.trans.z = (float)transform["translation"][1];

        // ��]�p
        objectData.rot.x = Math::Function::ToRadian(-(float)transform["rotation"][0]);
        objectData.rot.y = Math::Function::ToRadian(-(float)transform["rotation"][2]);
        objectData.rot.z = Math::Function::ToRadian((float)transform["rotation"][1]);

        // �X�P�[�����O
        objectData.scale.x = (float)transform["scaling"][0];
        objectData.scale.y = (float)transform["scaling"][2];
        objectData.scale.z = (float)transform["scaling"][1];

        if (jsonObject.contains("invisible_flag")) objectData.isInvisible = jsonObject["invisible_flag"].get<uint32_t>();
        else objectData.isInvisible = false;
    }

    // CAMERA
    if (type.compare("CAMERA") == 0) {
        // �v�f�ǉ�
        levelDataPtr->objects_.emplace_back(LevelData::ObjectData_t{});
        // ���ǉ������v�f�̎Q�Ƃ𓾂�
        LevelData::ObjectData_t& objectData = levelDataPtr->objects_.back();
        objectData.type = type;

        if (jsonObject.contains("file_name")) {
            // �t�@�C����
            objectData.file_name = jsonObject["file_name"];
        }

        objectData.name = jsonObject["name"].get<std::string>();

        // �g�����X�t�H�[���̃p�����[�^�ǂݍ���
        nlohmann::json& transform = jsonObject["transform"];

        // ���s�ړ�
        objectData.trans.x = (float)transform["translation"][0];
        objectData.trans.y = (float)transform["translation"][2];
        objectData.trans.z = (float)transform["translation"][1];

        // ��]�p
        objectData.rot.x = Math::Function::ToRadian(-(float)transform["rotation"][0]) + Math::Function::ToRadian(90.f);
        objectData.rot.y = Math::Function::ToRadian(-(float)transform["rotation"][2]);
        objectData.rot.z = Math::Function::ToRadian((float)transform["rotation"][1]);

        // �X�P�[�����O
        objectData.scale.x = (float)transform["scaling"][0];
        objectData.scale.y = (float)transform["scaling"][2];
        objectData.scale.z = (float)transform["scaling"][1];
    }
}
