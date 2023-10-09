#include "ModelManager.h"
#include "Util.h"
#include <fstream>
#include <sstream>

#pragma comment(lib,"d3d12.lib")

using namespace Util;
using VertexPosNormalUv_t = Mesh::VertexPosNormalUv_t;

void ModelManager::LoadOBJ(const fsPath& path, bool smoothing)
{
    // 既に読み込んだmodelデータとの重複確認。
    if (models_.count(path)) {
        Util::Log::PrintOutputWindow("A .obj file with the same name was loaded. Some models may not have been loaded.");
        return;
    }

    // 配列用の一時Model
    Model_t tempModel;
    tempModel.isSmoothing_ = smoothing;
    // 配列用の一時Mesh
    Mesh tempMesh;
    tempMesh.SetPath(path);

    // 頂点とインデックス情報
    std::ifstream ifs;
    ifs.open(path);
    assert(!ifs.fail());

    std::vector<Vector3> positions{}; // 頂点
    std::vector<Vector3> normals{};   // 法線
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

        // 頂点
        if (key == "v")
        {
            Vector3 pos;
            line_iss >> pos.x;
            line_iss >> pos.y;
            line_iss >> pos.z;

            positions.emplace_back(pos);
        }

        // テクスチャ
        if (key == "vt")
        {
            Vector2 texcoord{};
            line_iss >> texcoord.x;
            line_iss >> texcoord.y;

            texcoord.y = 1.0f - texcoord.y; // V方向反転
            texcoords.emplace_back(texcoord);
        }

        // 法線
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
                index_iss.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
                index_iss >> indexTexcoord;
                index_iss.seekg(1, std::ios_base::cur); // スラッシュを飛ばす
                index_iss >> indexNormal;

                VertexPosNormalUv_t vertex{};
                vertex.pos = positions[indexPos - 1];
                vertex.normal = normals[indexNormal - 1];
                vertex.uv = texcoords[indexTexcoord - 1];
                // 頂点データの追加
                vertices.emplace_back(vertex);
                // エッジ平滑化用データの追加
                if (smoothing) tempMesh.AddSmoothData(indexPos, (uint16_t)vertices.size() - 1);
                // 頂点インデックスデータの追加
                indices.emplace_back((uint16_t)indices.size());
                //-> indices.size()をemplace_back()することによって、追加された頂点データの要素数に合わせたインデックスが追加されていく
            }
        }
    }

    ifs.close();
    if (smoothing)tempMesh.CalcSmoothedVertNormals(vertices);

    // 頂点バッファ
    tempMesh.CreateVB(vertices);
    // インデックスバッファ
    tempMesh.CreateIB(indices);

    // 配列に追加
    meshes_.emplace(path, tempMesh);

    // 頂点データmeshの保管アドレスを持たせる。
    tempModel.meshPtr_ = &meshes_[path];
    // 配列に追加 ※定数バッファ生成はObject3Dのコンストラクタで行う。
    models_.emplace(path, tempModel);

}

void ModelManager::LoadMaterial(Model_t& model, const fsPath& path)
{
    const std::string mtlname{ path.stem().string() + ".mtl" };

    // ファイルストリーム
    std::ifstream file;
    // マテリアルファイルを開く
    file.open(path.parent_path().string() + "/" + mtlname);
    // ファイルオープン失敗をチェック
    assert(!file.fail());

    // 1行ずつ読み込む
    std::string line;
    while (std::getline(file, line)) {
        // 1行分の文字列をストリームに変換
        std::istringstream line_iss(line);

        // 半角スペース区切りで行の先頭文字列を取得
        std::string key;
        std::getline(line_iss, key, ' ');

        // 先頭のタブ文字は無視する
        if (key[0] == '\t') {
            key.erase(key.begin()); // 先頭の文字を削除
        }

        // 先頭文字列がnewmtlならマテリアル名
        if (key == "newmtl") {
            // マテリアル名読み込み
            line_iss >> model.material_.name;
        }

        // 先頭文字列がKaならアンビエント色
        if (key == "Ka") {
            line_iss >> model.material_.ambient.x;
            line_iss >> model.material_.ambient.y;
            line_iss >> model.material_.ambient.z;
        }

        // 先頭文字列ならKdならディフューズ色
        if (key == "Kd") {
            line_iss >> model.material_.diffuse.x;
            line_iss >> model.material_.diffuse.y;
            line_iss >> model.material_.diffuse.z;
        }

        // 先頭文字列がKsならスペキュラー色
        if (key == "Ks") {
            line_iss >> model.material_.specular.x;
            line_iss >> model.material_.specular.y;
            line_iss >> model.material_.specular.z;
        }

        // 先頭文字列がmap_Kdならテクスチャファイル名
        if (key == "map_Kd") {
            std::string tmpName;

            // テクスチャのファイル名読み込み
            line_iss >> tmpName;
            model.material_.texKey = path.parent_path() / tmpName;

            // テクスチャ読み込み
            texMPtr_->Load(model.material_.texKey);
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
        // エラーログ
        Util::Log::PrintOutputWindow("[ModelManager] : Couldn't find a 3Dmodel corresponding to the argument path(" + path.string() + "), so replaced it with \"cube\". ");
        // 指定されたモデルが見つからなかった場合cubeに置き換えて返却する
        return models_["Resources/cube/cube.obj"];
    }
}

void Model_t::UpdateCB(void)
{
    // 定数バッファのマテリアルの値を更新する
    cbMaterial_.GetConstBuffMap()->ambient = material_.ambient;
    cbMaterial_.GetConstBuffMap()->diffuse = material_.diffuse;
    cbMaterial_.GetConstBuffMap()->specular = material_.specular;
    cbMaterial_.GetConstBuffMap()->alpha = material_.alpha;
}
