#pragma once
#include <fstream>
#include <xaudio2.h>
#include <wrl.h>

#pragma comment(lib, "xaudio2.lib")

class Sound;

class Audio {
public:
    Audio() = default;
    ~Audio() { Finalize(); }

    bool Initialize();
    void Finalize();

    void Play(const Sound& sound);

private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masterVoice_ = nullptr;
};
