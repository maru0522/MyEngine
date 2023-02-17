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

    void Register(const Image& img);
    void Register(const Image& img, const std::string& nickname, bool isAddNickname = true);

private:
    void GenerateMissingImage(void); // エラー画像の生成と登録

    // 変数
    std::map<std::string, fsPath> paths_{}; // ニックネームと所属を紐付け。
    std::map<fsPath, Image> textures_{}; // 所属とImageを紐付け。

    InitDirectX* p_idx_{ nullptr };
};

