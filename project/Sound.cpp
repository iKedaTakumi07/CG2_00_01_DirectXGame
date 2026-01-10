#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")

#include "Sound.h"
#include "StringUtility.h"
#include <cassert>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <wrl.h>

void Sound::SoundLoadFile(const std::string& filename)
{

    // フルパスをワイド文字に変換
    std::wstring filePathW = StringUtility::ConvertString(filename);
    HRESULT result;

    // SoundReader作成
    Microsoft::WRL::ComPtr<IMFSourceReader> pReader;
    result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &pReader);
    assert(SUCCEEDED(result));

    // PCM形式にフォーマット指定する
    Microsoft::WRL::ComPtr<IMFMediaType> pPCMType;
    MFCreateMediaType(&pPCMType);
    pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    result = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pPCMType.Get());
    assert(SUCCEEDED(result));

    // 実際にセットされたメディアタイプを取得する
    Microsoft::WRL::ComPtr<IMFMediaType> pOutType;
    pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);

    // Waveフォーマットを取得する
    WAVEFORMATEX* waveFormat = nullptr;
    MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);

    // コンテナに格納する音声データ
    soundData.wfex = *waveFormat;

    CoTaskMemFree(waveFormat);

    // PCMデータのバッファを構築
    while (true) {
        Microsoft::WRL::ComPtr<IMFSample> pSample;
        DWORD streamIndex = 0, flags = 0;
        LONGLONG llTimeStamp = 0;
        // サンプルを読み込む
        result = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
        // ストリームの末尾に達したら抜ける
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
        if (pSample) {
            Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
            // サンプルに含まれるサウンドデータのバッファを一繋ぎにして取得
            pSample->ConvertToContiguousBuffer(&pBuffer);

            BYTE* pData = nullptr;
            DWORD maxLength = 0, currentLength = 0;
            // バッファ読み込み用にロック
            pBuffer->Lock(&pData, &maxLength, &currentLength);
            // バッファの末尾にデータを追加
            soundData.buffer.insert(soundData.buffer.end(), pData, pData + currentLength);
            pBuffer->Unlock();
        }
    }

    //// ファイル入力ストリームのインスタンス
    // std::ifstream file;
    //// .wavファイルをバイナリモードで開く
    // file.open(filename, std::ios_base::binary);
    //// ファイルオープン失敗を検出する
    // assert(file.is_open());

    // RiffHeader riff;
    // file.read((char*)&riff, sizeof(riff));
    // if (strncmp(riff.chunk.id, "RIFF", 4) != 0 || strncmp(riff.type, "WAVE", 4) != 0) {
    //     assert(0);
    // }

    //// Formatチャンクの読み込み
    // FormatChunk format = {};
    //// チャンクヘッダーの確認
    // file.read((char*)&format, sizeof(ChunkHeader));
    // if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
    //     assert(0);
    // }
    //// チャンク本体の読み込み
    // assert(format.chunk.size <= sizeof(format.fmt));
    // file.read((char*)&format.fmt, format.chunk.size);

    // ChunkHeader data {};
    // file.read((char*)&data, sizeof(data));

    // if (strncmp(data.id, "JUNK", 4) == 0) {
    //     file.seekg(data.size, std::ios::cur);
    //     file.read((char*)&data, sizeof(data));
    // }

    // if (strncmp(data.id, "data", 4) != 0) {
    //     assert(0);
    // }

    //soundData.pBuffer = new BYTE[data.size];
    //soundData.bufferSize = data.size;
    //soundData.wfex = format.fmt;

    //file.read((char*)soundData.pBuffer, data.size);

    //// waveファイルを閉じる
    //file.close();
}

void Sound::Unload()
{
    soundData.buffer.clear();
    soundData.wfex = {};
}
