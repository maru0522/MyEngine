#include "Sound.h"
#include <cassert>
#include "MathUtil.h"

AudioManager* Sound::sAudioMPtr_{ nullptr };

void Sound::StaticInitalize(AudioManager* audioMPtr)
{
    sAudioMPtr_ = audioMPtr;
}

Sound::Sound(const fsPath& path) : soundPtr_(nullptr), volume_(1), pitch_(1)
{
    soundPtr_ = sAudioMPtr_->GetSoundPtr(path);

    // nullチェック
    assert(sAudioMPtr_->GetSoundPtr(path) && "[Sound] : soundPtrがnullptrです。");
}

void Sound::Play(uint32_t count, Loop type)
{
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = reinterpret_cast<BYTE*>(soundPtr_->pBuffer.data());
    buf.AudioBytes = soundPtr_->bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    if (type == Loop::LIMIT) buf.LoopCount = count;                     // 回数ループ
    if (type == Loop::UNLIMIT) buf.LoopCount = XAUDIO2_LOOP_INFINITE;   // 無限ループ

    HRESULT hr = soundPtr_->pSourceVoice->SubmitSourceBuffer(&buf);
    hr = soundPtr_->pSourceVoice->Start();
    assert(SUCCEEDED(hr));
}

void Sound::Stop(void)
{
    soundPtr_->pSourceVoice->Stop();
}

bool Sound::GetPlaying(void)
{
    XAUDIO2_VOICE_STATE state{};
    soundPtr_->pSourceVoice->GetState(&state);
    return state.SamplesPlayed != 0;
}

void Sound::SetVolume(float volume)
{
    volume_ = Math::Function::Clamp<float>(volume, 0.f, 1.f);
    soundPtr_->pSourceVoice->SetVolume(volume_);
}

void Sound::SetPitch(float pitch)
{
    pitch_ = Math::Function::Clamp<float>(pitch, 0.f, 2.f);
    soundPtr_->pSourceVoice->SetFrequencyRatio(pitch_);
}
