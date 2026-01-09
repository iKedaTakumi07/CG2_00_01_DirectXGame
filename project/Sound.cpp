#include "Sound.h"
#include <cassert>

bool Sound::Load(const char* filename)
{
    // ファイル入力ストリームのインスタンス
    std::ifstream file;
    // .wavファイルをバイナリモードで開く
    file.open(filename, std::ios_base::binary);
    // ファイルオープン失敗を検出する
    assert(file.is_open());

    RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));
    if (strncmp(riff.chunk.id, "RIFF", 4) != 0 || strncmp(riff.type, "WAVE", 4) != 0) {
        assert(0);
    }

    // Formatチャンクの読み込み
    FormatChunk format = {};
    // チャンクヘッダーの確認
    file.read((char*)&format, sizeof(ChunkHeader));
    if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
        assert(0);
    }
    // チャンク本体の読み込み
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);

    ChunkHeader data {};
    file.read((char*)&data, sizeof(data));

    if (strncmp(data.id, "JUNK", 4) == 0) {
        file.seekg(data.size, std::ios::cur);
        file.read((char*)&data, sizeof(data));
    }

    if (strncmp(data.id, "data", 4) != 0) {
        assert(0);
    }

    pBuffer_ = new BYTE[data.size];
    bufferSize_ = data.size;
    file.read((char*)pBuffer_, data.size);

    // waveファイルを閉じる
    file.close();

    wfex_ = format.fmt;
    return true;
}

void Sound::Unload()
{
    delete[] pBuffer_;
    pBuffer_ = nullptr;
    bufferSize_ = 0;
    wfex_ = {};
}
