#pragma comment(lib, "mfplat.lib")

#include <mfapi.h>
#include "Audio.h"
#include "Sound.h"
#include <cassert>

bool Audio::Initialize()
{
    HRESULT result;

    result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
    assert(SUCCEEDED(result));

    HRESULT hr = XAudio2Create(&xAudio2_, 0);
    if (FAILED(hr))
        return false;

    hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
    return SUCCEEDED(hr);
}

void Audio::Finalize()
{
    if (masterVoice_) {
        masterVoice_->DestroyVoice();
        masterVoice_ = nullptr;
    }
    xAudio2_.Reset();

    HRESULT result;

    result = MFShutdown();
    assert(SUCCEEDED(result));
}

void Audio::Play(const Sound& sound)
{
    IXAudio2SourceVoice* sourceVoice = nullptr;

    HRESULT hr = xAudio2_->CreateSourceVoice(
        &sourceVoice,
        &sound.GetFormat());
    assert(SUCCEEDED(hr));

    XAUDIO2_BUFFER buf {};
    buf.pAudioData = sound.GetBuffer();
    buf.AudioBytes = sound.GetBufferSize();
    buf.Flags = XAUDIO2_END_OF_STREAM;

    sourceVoice->SubmitSourceBuffer(&buf);
    sourceVoice->Start();
}