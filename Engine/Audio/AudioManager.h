#pragma once
#include <map>
#include <wrl.h>
#include <fstream>
#include <filesystem>
#include <xaudio2.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

class AudioManager final
{
private:
    // 定義
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::filesystem::path;

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
        std::vector<BYTE> pBuffer{};
        IXAudio2SourceVoice* pSourceVoice{};
    };

public:
    // 関数
    AudioManager(void);
    ~AudioManager(void);

    void Load(const fsPath& path);
    void LoadFolder(const fsPath& path);
    inline SoundData_t* GetSoundPtr(const fsPath& path) { return &soundDatum_[path]; }

private:
    void LoadMp3(const fsPath& path);
    void LoadWave(const fsPath& path);
    void SkipHeader(std::ifstream& file, ChunkHeader_t& chunkHead, const char* chunkId, size_t cmpNum = 4);

    // 変数
    // XAudio2
    ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masterVoice_;

    // MediaFoundation
    ComPtr<IMFSourceReader> mFSourceReader_;
    ComPtr<IMFMediaBuffer> mFMediaBuffer_;
    ComPtr<IMFMediaType> mFMediaType_;
    ComPtr<IMFSample> mFSample_;

    std::map<fsPath, SoundData_t> soundDatum_;
};

