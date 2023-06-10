#include "AudioManager.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

AudioManager::AudioManager(void)
{
    // XAudio2������
    HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    hr = xAudio2_->CreateMasteringVoice(&masterVoice_);

    // Media Foundation������
    hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

AudioManager::~AudioManager(void)
{
    xAudio2_.Reset();
    MFShutdown();
}

void AudioManager::Load(const fsPath& path)
{
    // ���ɓǂݍ���audio�f�[�^�Ƃ̏d���m�F�B
    if (soundDatum_.count(path)) {
        Util::Log::PrintOutputWindow("An audio file with the same name was loaded. Some audio files may not have been loaded.");
        return;
    }

    if (path.extension() == ".wav") LoadWave(path);
    if (path.extension() == ".mp3") LoadMp3(path);
}

void AudioManager::LoadFolder(const fsPath& path)
{
    for (const std::experimental::filesystem::directory_entry& file : std::experimental::filesystem::directory_iterator(path)) {
        // �t�@�C�����擾
        std::experimental::filesystem::path fileName{ file.path().filename().string() };
        Load(path / fileName);
    }
}

void AudioManager::LoadMp3(const fsPath& path)
{
    std::wstring wPath{ path.c_str() };

    // �\�[�X���[�_�[�̐���
    HRESULT hr = MFCreateSourceReaderFromURL(path.c_str(), NULL, mFSourceReader_.GetAddressOf());
    assert(SUCCEEDED(hr));

    // ���f�B�A�^�C�v�̎擾
    MFCreateMediaType(mFMediaType_.GetAddressOf());
    mFMediaType_->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    mFMediaType_->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    mFSourceReader_->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, mFMediaType_.Get());

    mFMediaType_.Reset();
    mFSourceReader_->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, mFMediaType_.GetAddressOf());

    // �I�[�f�B�I�f�[�^�`���̍쐬
    WAVEFORMATEX* wfex{};
    MFCreateWaveFormatExFromMFMediaType(mFMediaType_.Get(), &wfex, nullptr);

    // �f�[�^�̓ǂݍ���
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

    // �\�[�X�{�C�X�̐���
    hr = xAudio2_->CreateSourceVoice(&soundDatum_[path].pSourceVoice, &soundDatum_[path].wfex);
    assert(SUCCEEDED(hr));
}

void AudioManager::LoadWave(const fsPath& path)
{
    std::ifstream file{ path, std::ios_base::binary };
    assert(file.is_open());

    // "RIFF"�`�����N�ǂݍ���
    RiffHeader_t riff{};
    file.read((char*)&riff, sizeof(riff));
    if (std::strncmp(riff.chunk.id, "RIFF", 4) != 0) assert(0);     // �`�����N�F"RIFF"�ȊO�̎��G���[
    if (std::strncmp(riff.type, "WAVE", 4) != 0) assert(0);         // �^�C�v�@�F"WAVE"�ȊO�̎��G���[

    // "fmt "�`�����N�ǂݍ���
    FormatChunk_t format{};
    file.read((char*)&format, sizeof(ChunkHeader_t));
    SkipHeader(file, format.chunk, "LIST");             // �`�����N�� "LIST" ���������X�L�b�v
    SkipHeader(file, format.chunk, "JUNK");             // �`�����N�� "JUNK" ���������X�L�b�v

    if (std::strncmp(format.chunk.id, "fmt ", 4) != 0) assert(0);   // �`�����N�F"fmt "�ȊO�̎��G���[
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);

    // "data" �`�����N�ǂݍ���
    ChunkHeader_t data{};
    file.read((char*)&data, sizeof(data));
    SkipHeader(file, data, "LIST");             // �`�����N�� "LIST" ���������X�L�b�v
    SkipHeader(file, data, "JUNK");             // �`�����N�� "JUNK" ���������X�L�b�v
    SkipHeader(file, data, "bext");             // �`�����N�� "bext" ���������X�L�b�v

    if (std::strncmp(data.id, "REAPER", 6) == 0) assert(0);         // REAPER(DAW)���g���č���Ă���ꍇ,�ǂݍ��߂Ȃ��̂ŃG���[
    if (std::strncmp(data.id, "data", 4) != 0) assert(0);           // �`�����N�F"data"�ȊO�̎��G���[
    std::vector<char> pBuffer(data.size);
    file.read(pBuffer.data(), data.size);
    SoundData_t tempSound{ format.fmt,  (uint32_t)data.size, std::vector<BYTE>(pBuffer.begin(),pBuffer.end()) };
    soundDatum_.insert({ path, tempSound });

    // �\�[�X�{�C�X�̐���
    HRESULT hr = xAudio2_->CreateSourceVoice(&soundDatum_[path].pSourceVoice, &soundDatum_[path].wfex);
    assert(SUCCEEDED(hr));

    file.close();
}

void AudioManager::SkipHeader(std::ifstream& file, ChunkHeader_t& chunkHead, const char* chunkId, size_t cmpNum)
{
    // �ǂݍ��񂾃`�����N���w�肵��id���������i��FJUNK
    if (strncmp(chunkHead.id, chunkId, cmpNum) == 0) {
        // �ǂݎ��ʒu�𓖊Y�`�����N�̍Ō�܂ňړ�
        file.seekg(chunkHead.size, std::ios_base::cur);
        // �ēǂݍ���
        file.read((char*)&chunkHead, sizeof(chunkHead));
    }
}
