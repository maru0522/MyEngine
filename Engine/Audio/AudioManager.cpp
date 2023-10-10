#include "AudioManager.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

AudioManager::AudioManager(void)
{
    // XAudio2初期化
    HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    hr = xAudio2_->CreateMasteringVoice(&masterVoice_);

    // Media Foundation初期化
    hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

AudioManager::~AudioManager(void)
{
    xAudio2_.Reset();
    MFShutdown();
}

void AudioManager::Load(const fsPath& path)
{
    // 既に読み込んだaudioデータとの重複確認。
    if (soundDatum_.count(path)) {
        Util::Log::PrintOutputWindow("An audio file with the same name was loaded. Some audio files may not have been loaded.");
        return;
    }

    if (path.extension() == ".wav") LoadWave(path);
    if (path.extension() == ".mp3") LoadMp3(path);
}

void AudioManager::LoadFolder(const fsPath& path)
{
    for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(path)) {
        // ファイル名取得
        std::filesystem::path fileName{ file.path().filename().string() };
        Load(path / fileName);
    }
}

void AudioManager::LoadMp3(const fsPath& path)
{
    std::wstring wPath{ path.c_str() };

    // ソースリーダーの生成
    HRESULT hr = MFCreateSourceReaderFromURL(path.c_str(), NULL, mFSourceReader_.GetAddressOf());
    assert(SUCCEEDED(hr));

    // メディアタイプの取得
    MFCreateMediaType(mFMediaType_.GetAddressOf());
    mFMediaType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    mFMediaType_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    mFSourceReader_->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, mFMediaType_.Get());

    mFMediaType_.Reset();
    mFSourceReader_->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, mFMediaType_.GetAddressOf());

    // オーディオデータ形式の作成
    WAVEFORMATEX* wfex{};
    MFCreateWaveFormatExFromMFMediaType(mFMediaType_.Get(), &wfex, nullptr);

    // データの読み込み
    std::vector<BYTE> mediaData;
    while (true)
    {
        DWORD dwStreamFlags{};
        mFSourceReader_->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, mFSample_.GetAddressOf());

        if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) break;

        mFSample_->ConvertToContiguousBuffer(mFMediaBuffer_.GetAddressOf());

        BYTE* pBuffer{};
        DWORD cbCurrentLength{};
        mFMediaBuffer_->Lock(&pBuffer, nullptr, &cbCurrentLength);

        mediaData.resize(mediaData.size() + cbCurrentLength);
        memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

        mFMediaBuffer_->Unlock();

        mFMediaBuffer_.Reset();
        mFSample_.Reset();
    }
    SoundData_t tempSound{ *wfex, sizeof(BYTE) * static_cast<uint32_t>(mediaData.size()), mediaData };
    soundDatum_.insert({ path,tempSound });

    CoTaskMemFree(wfex);
    mFSourceReader_.Reset();
    mFMediaType_.Reset();

    // ソースボイスの生成
    hr = xAudio2_->CreateSourceVoice(&soundDatum_[path].pSourceVoice, &soundDatum_[path].wfex);
    assert(SUCCEEDED(hr));
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

    if (std::strncmp(data.id, "REAPER", 6) == 0) assert(0);         // REAPER(DAW)を使って作られている場合,読み込めないのでエラー
    if (std::strncmp(data.id, "data", 4) != 0) assert(0);           // チャンク："data"以外の時エラー
    std::vector<char> pBuffer(data.size);
    file.read(pBuffer.data(), data.size);
    SoundData_t tempSound{ format.fmt,  (uint32_t)data.size, std::vector<BYTE>(pBuffer.begin(),pBuffer.end()) };
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
