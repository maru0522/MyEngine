#include "AudioManager.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

AudioManager::AudioManager(void)
{
    HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

AudioManager::~AudioManager(void)
{
    xAudio2_.Reset();

    //for (auto& sound : soundDatum_)
    //{
    //    delete[] sound.second.pBuffer;

    //    sound.second.wfex = {};
    //    sound.second.pBuffer = 0;
    //    sound.second.bufferSize = 0;
    //}

    //masterVoice_->DestroyVoice();
}

void AudioManager::Load(const fsPath& path)
{
    // 既に読み込んだaudioデータとの重複確認。
    if (soundDatum_.count(path)) {
        Util::Log::PrintOutputWindow("An audio file with the same name was loaded. Some audio files may not have been loaded.");
        return;
    }

    if (path.extension() == ".wav") LoadWave(path);
}

void AudioManager::LoadFolder(const fsPath& path)
{
    for (const std::experimental::filesystem::directory_entry& file : std::experimental::filesystem::directory_iterator(path)) {
        std::experimental::filesystem::path fileName{ file.path().filename().string() };

        if (file.path().extension() == ".wav") {
            Load(path / fileName);
        }
    }
}

void AudioManager::LoadWave(const fsPath& path)
{
    std::ifstream file{ path, std::ios_base::binary };
    assert(file.is_open());

    // "RIFF"チャンク読み込み
    RiffHeader_t riff{};
    file.read((char*)&riff, sizeof(riff));
    if (std::strncmp(riff.chunk.id, "RIFF", 4) != 0) assert(0);     // チャンク："RIFF"以外の時エラー
    if (std::strncmp(riff.type, "WAVE", 4) != 0) assert(0);         // タイプ　："WAVE"以外の時エラー

    // "fmt "チャンク読み込み
    FormatChunk_t format{};
    file.read((char*)&format, sizeof(ChunkHeader_t));
    SkipHeader(file, format.chunk, "LIST");             // チャンクが "LIST" だった時スキップ
    SkipHeader(file, format.chunk, "JUNK");             // チャンクが "JUNK" だった時スキップ

    if (std::strncmp(format.chunk.id, "fmt ", 4) != 0) assert(0);   // チャンク："fmt "以外の時エラー
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);

    // "data" チャンク読み込み
    ChunkHeader_t data{};
    file.read((char*)&data, sizeof(data));
    SkipHeader(file, data, "LIST");             // チャンクが "LIST" だった時スキップ
    SkipHeader(file, data, "JUNK");             // チャンクが "JUNK" だった時スキップ
    SkipHeader(file, data, "bext");             // チャンクが "bext" だった時スキップ

    if (std::strncmp(data.id, "data", 4) != 0) assert(0);           // チャンク："data"以外の時エラー
    std::vector<char> pBuffer(data.size);
    file.read(pBuffer.data(), data.size);
    SoundData_t tempSound{ format.fmt,  (uint32_t)data.size, pBuffer };
    soundDatum_.insert({ path, tempSound });

    // ソースボイスの生成
    HRESULT hr = xAudio2_->CreateSourceVoice(&soundDatum_[path].pSourceVoice, &soundDatum_[path].wfex);
    assert(SUCCEEDED(hr));

    file.close();
}

void AudioManager::SkipHeader(std::ifstream& file, ChunkHeader_t& chunkHead, const char* chunkId, size_t cmpNum)
{
    // 読み込んだチャンクが指定したidだった時（例：JUNK
    if (strncmp(chunkHead.id, chunkId, cmpNum) == 0) {
        // 読み取り位置を当該チャンクの最後まで移動
        file.seekg(chunkHead.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&chunkHead, sizeof(chunkHead));
    }
}
