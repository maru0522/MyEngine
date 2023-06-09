#pragma once
#include <map>
#include <wrl.h>
#include <fstream>
#include "FileSystem.h"
#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")

class AudioManager
{
private:
    // ��`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

    struct ChunkHeader_t
    {
        char id[4];
        int32_t size;
    };

    struct RiffHeader_t
    {
        ChunkHeader_t chunk;
        char type[4];
    };

    struct FormatChunk_t
    {
        ChunkHeader_t chunk;
        WAVEFORMATEX fmt;
    };

public:
    struct SoundData_t
    {
        WAVEFORMATEX wfex{};
        uint32_t bufferSize{};
        std::vector<char> pBuffer{};
        IXAudio2SourceVoice* pSourceVoice{};
    };

public:
    // �֐�
    AudioManager(void);
    ~AudioManager(void);

    void Load(const fsPath& path);
    void LoadFolder(const fsPath& path);
    inline SoundData_t* GetSoundPtr(const fsPath& path) { return &soundDatum_[path]; }

private:
    void LoadWave(const fsPath& path);
    void SkipHeader(std::ifstream& file, ChunkHeader_t& chunkHead, const char* chunkId, size_t cmpNum = 4);

    // �ϐ�
    ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masterVoice_;
    std::map<fsPath, SoundData_t> soundDatum_;
};

