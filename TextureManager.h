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
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

    struct Image
    {
        // ����
        fsPath path_{};

        // �o�b�t�@
        ComPtr<ID3D12Resource> buff_{ nullptr };
        // srv��gpu�n���h��
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_{};
    };

public:
    // �֐�
    void Initialize(InitDirectX* p_idx);

    // �w�肵���摜��ǂݍ���
    void Load(const fsPath& path);
    void Load(const fsPath& path, const std::string& nickname);

    // �w�肵���t�H���_�̉摜��S�ēǂݍ���(.png/.jpg/.jepg�̂�)
    void LoadFolder(const fsPath& path);

    // �ۊǂ���Ă�Image��Ή�����path�ň����o���B
    const Image& GetImage(const fsPath& path) const;

private:
    // �o���オ��������[Image]��map�R���e�i�ɓo�^����
    void RegisterImg(const Image& img);
    void RegisterNickname(const fsPath& path, const std::string& nickname);

    void GenerateMissingImage(void); // �G���[�摜�̐����Ɠo�^

    bool CheckDuplicateTexture(const Image& img);
    bool CheckDuplicateNickname(const std::string& nickname);

    // �ϐ�
    std::map<std::string, fsPath> paths_{}; // �j�b�N�l�[���Ə�����R�t���B
    std::map<fsPath, Image> textures_{}; // ������Image��R�t���B

    InitDirectX* p_idx_{ nullptr };
};

