#pragma once
#if _MSC_VER > 1922 && !defined(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>
#include <string>
#include <map>
#include <wrl.h>
#include <d3d12.h>

#include "InitDirectX.h"

class TextureManager
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

    struct Image
    {
        // 所属
        fsPath path_{};

        // バッファ
        ComPtr<ID3D12Resource> buff_{ nullptr };
        // srvのgpuハンドル
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_{};
    };

public:
    // 関数
    void Initialize(InitDirectX* p_idx);

    // 指定した画像を読み込み
    void Load(const fsPath& path);
    void Load(const fsPath& path, const std::string& nickname);

    // 指定したフォルダの画像を全て読み込み(.png/.jpg/.jepgのみ)
    void LoadFolder(const fsPath& path);

    // 保管されてるImageを対応するpathで引き出す。
    const Image& GetImage(const fsPath& path) const;

private:
    // 出来上がった情報体[Image]をmapコンテナに登録する
    void RegisterImg(const Image& img);
    void RegisterNickname(const fsPath& path, const std::string& nickname);

    void GenerateMissingImage(void); // エラー画像の生成と登録

    bool CheckDuplicateTexture(const Image& img);
    bool CheckDuplicateNickname(const std::string& nickname);

    // 変数
    std::map<std::string, fsPath> paths_{}; // ニックネームと所属を紐付け。
    std::map<fsPath, Image> textures_{}; // 所属とImageを紐付け。

    InitDirectX* p_idx_{ nullptr };
};

