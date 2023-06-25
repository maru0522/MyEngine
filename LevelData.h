#pragma once
#include <string>
#include "json.hpp"
#include "Vector3.h"
#include "Matrix4.h"
#include "ModelManager.h"

class LevelData
{
public:
    static LevelData* Load(const std::string& path);
    static void ScanRecursive(nlohmann::json& jsonObject, LevelData* levelDataPtr);
    struct ObjectData_t
    {
        std::string name;
        std::string type;
        Vector3 trans;
        Vector3 scale;
        Vector3 rot;
        Matrix4 matWorld;
    };

    std::list<ObjectData_t> objects_;
};

