#include "LevelData.h"
#include "MathUtil.h"
#include <fstream>
#include <cassert>

std::unique_ptr<LevelData> LevelData::Load(const std::string& path)
{
    std::ifstream ifs;
    ifs.open(path);
    assert(!ifs.fail());

    // json文字列のデータ
    nlohmann::json deserialized;

    // 解凍
    ifs >> deserialized;

    // 正しいレベルデータファイルかチェック
    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].is_string());

    // "name"を文字列として取得
    std::string name = deserialized["name"].get<std::string>();
    // 正しいレベルデータファイルかチェック
    assert(name.compare("scene") == 0);

    // レベルデータ格納用インスタンス生成
    std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

    // objectsの全オブジェクトを走査
    for (nlohmann::json& object : deserialized["objects"]) {
        ScanRecursive(object, levelData.get());

        // 再起呼出で枝を走査する
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

    // 種別を取得
    std::string type = jsonObject["type"].get<std::string>();

    // MESH
    if (type.compare("MESH") == 0) {
        // 要素追加
        levelDataPtr->objects_.emplace_back(LevelData::ObjectData_t{});
        // 今追加した要素の参照を得る
        LevelData::ObjectData_t& objectData = levelDataPtr->objects_.back();
        objectData.type = type;

        if (jsonObject.contains("file_name")) {
            // ファイル名
            objectData.file_name = jsonObject["file_name"];
        }

        objectData.name = jsonObject["name"].get<std::string>();

        // トランスフォームのパラメータ読み込み
        nlohmann::json& transform = jsonObject["transform"];

        // 平行移動
        objectData.trans.x = (float)transform["translation"][0];
        objectData.trans.y = (float)transform["translation"][2];
        objectData.trans.z = (float)transform["translation"][1];

        // 回転角
        objectData.rot.x = Math::Function::ToRadian(-(float)transform["rotation"][0]);
        objectData.rot.y = Math::Function::ToRadian(-(float)transform["rotation"][2]);
        objectData.rot.z = Math::Function::ToRadian((float)transform["rotation"][1]);

        // スケーリング
        objectData.scale.x = (float)transform["scaling"][0];
        objectData.scale.y = (float)transform["scaling"][2];
        objectData.scale.z = (float)transform["scaling"][1];

        if (jsonObject.contains("invisible_flag")) objectData.isInvisible = jsonObject["invisible_flag"].get<uint32_t>();
        else objectData.isInvisible = false;
    }

    // LIGHT
    if (type.compare("LIGHT") == 0) {
        // 要素追加
        levelDataPtr->objects_.emplace_back(LevelData::ObjectData_t{});
        // 今追加した要素の参照を得る
        LevelData::ObjectData_t& objectData = levelDataPtr->objects_.back();
        objectData.type = type;

        if (jsonObject.contains("file_name")) {
            // ファイル名
            objectData.file_name = jsonObject["file_name"];
        }

        objectData.name = jsonObject["name"].get<std::string>();

        // トランスフォームのパラメータ読み込み
        nlohmann::json& transform = jsonObject["transform"];

        // 平行移動
        objectData.trans.x = (float)transform["translation"][0];
        objectData.trans.y = (float)transform["translation"][2];
        objectData.trans.z = (float)transform["translation"][1];

        // 回転角
        objectData.rot.x = Math::Function::ToRadian(-(float)transform["rotation"][0]);
        objectData.rot.y = Math::Function::ToRadian(-(float)transform["rotation"][2]);
        objectData.rot.z = Math::Function::ToRadian((float)transform["rotation"][1]);

        // スケーリング
        objectData.scale.x = (float)transform["scaling"][0];
        objectData.scale.y = (float)transform["scaling"][2];
        objectData.scale.z = (float)transform["scaling"][1];

        if (jsonObject.contains("invisible_flag")) objectData.isInvisible = jsonObject["invisible_flag"].get<uint32_t>();
        else objectData.isInvisible = false;
    }

    // CAMERA
    if (type.compare("CAMERA") == 0) {
        // 要素追加
        levelDataPtr->objects_.emplace_back(LevelData::ObjectData_t{});
        // 今追加した要素の参照を得る
        LevelData::ObjectData_t& objectData = levelDataPtr->objects_.back();
        objectData.type = type;

        if (jsonObject.contains("file_name")) {
            // ファイル名
            objectData.file_name = jsonObject["file_name"];
        }

        objectData.name = jsonObject["name"].get<std::string>();

        // トランスフォームのパラメータ読み込み
        nlohmann::json& transform = jsonObject["transform"];

        // 平行移動
        objectData.trans.x = (float)transform["translation"][0];
        objectData.trans.y = (float)transform["translation"][2];
        objectData.trans.z = (float)transform["translation"][1];

        // 回転角
        objectData.rot.x = Math::Function::ToRadian(-(float)transform["rotation"][0]) + Math::Function::ToRadian(90.f);
        objectData.rot.y = Math::Function::ToRadian(-(float)transform["rotation"][2]);
        objectData.rot.z = Math::Function::ToRadian((float)transform["rotation"][1]);

        // スケーリング
        objectData.scale.x = (float)transform["scaling"][0];
        objectData.scale.y = (float)transform["scaling"][2];
        objectData.scale.z = (float)transform["scaling"][1];
    }
}
