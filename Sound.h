#pragma once
#include <wrl.h>
#include "FileSystem.h"
#include "AudioManager.h"
#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")

class Sound
{
private:
    // íËã`
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using fsPath = std::experimental::filesystem::path;

public:
    enum class Loop
    {
        LIMIT,
        UNLIMIT,
    };

    // ä÷êî
    static void StaticInitalize(AudioManager* audioMPtr);
    Sound(const fsPath& path);

    void Play(uint32_t count = 0u, Loop type = Loop::LIMIT);
    void Pause(void);
    void Stop(void);
    bool GetPlaying(void);
    void SetVolume(float volume);
    void SetPitch(float pitch);

private:
    // ïœêî
    static AudioManager* sAudioMPtr_;

    AudioManager::SoundData_t* soundPtr_;
    float volume_;
    float pitch_;
};
