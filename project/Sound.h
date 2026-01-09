#pragma once
#include <fstream>
#include <xaudio2.h>

#pragma comment(lib, "xaudio2.lib")

struct ChunkHeader {
    char id[4]; // チャンク毎のID
    int32_t size; // チャンクサイズ
};
struct RiffHeader {
    ChunkHeader chunk; // "RIFF"
    char type[4]; // "WAVE"
};
struct FormatChunk {
    ChunkHeader chunk; // "fmt"
    WAVEFORMATEX fmt; // 波型フォーマット
};
struct SoundData {
    // 波型フォーマット
    WAVEFORMATEX wfex;
    // バッフアの先頭アドレス
    BYTE* pBuffer;
    // バッフアのサイズ
    unsigned int bufferSize;
};

class Sound {
public:
    Sound() = default;
    ~Sound() { Unload(); }

    bool Load(const char* filename);
    void Unload();

    const WAVEFORMATEX& GetFormat() const { return wfex_; }
    const BYTE* GetBuffer() const { return pBuffer_; }
    UINT32 GetBufferSize() const { return bufferSize_; }

private:
    WAVEFORMATEX wfex_ {};
    BYTE* pBuffer_ = nullptr;
    UINT32 bufferSize_ = 0;
};
