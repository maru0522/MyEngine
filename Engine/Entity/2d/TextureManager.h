#pragma once
#include "FileSystem.h"
#include <string>
#include <map>
#include <wrl.h>
#include <d3d12.h>

#include "InitDirectX.h"

class TextureManager final
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

public:
    struct Image
    {
        // 所属
        fsPath path{};

        // バッファ
        ComPtr<ID3D12Resource> buff{ nullptr };
        // srvのgpuハンドル
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle{};
    };

    // 関数
    void Initialize(void);

    // 指定した画像を読み込み
    void Load(const fsPath& path);
    void Load(const fsPath& path, const std::string& nickname);

    // 指定したフォルダの画像を全て読み込み(.png/.jpg/.jepgのみ)
    void LoadFolder(const fsPath& path);

    // 保管されてるImageを対応するpathで引き出す。
    const Image* GetImagePtr(const fsPath& path) const;
    // 保管されてるImageを対応するnicknameで引き出す。
    const Image* GetImagePtrByNickname(const std::string& nickname);

private:
    // 出来上がった情報体[Image]をmapコンテナに登録する
    void RegisterImg(const Image& img);
    void RegisterNickname(const fsPath& path, const std::string& nickname);

    void GenerateMissingImage(void); // エラー画像の生成と登録

    bool CheckDuplicateTexture(const Image& img);
    bool CheckDuplicateNickname(const std::string& nickname);

    // 変数
    std::map<std::string, fsPath> paths_{}; // ニックネームとkeyを紐付け。
    std::map<fsPath, Image> textures_{}; // keyとImageを紐付け。
};
