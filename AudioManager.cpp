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
    // ���ɓǂݍ���audio�f�[�^�Ƃ̏d���m�F�B
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

    if (std::strncmp(data.id, "data", 4) != 0) assert(0);           // �`�����N�F"data"�ȊO�̎��G���[
    std::vector<char> pBuffer(data.size);
    file.read(pBuffer.data(), data.size);
    SoundData_t tempSound{ format.fmt,  (uint32_t)data.size, pBuffer };
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
