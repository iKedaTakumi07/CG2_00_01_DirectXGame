#define DIRECTINPUT_VERSION 0x0800

#include "Audio.h"
#include "D3dResourceLeakChecker.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Logger.h"
#include "Math.h"
#include "Object3dCommon.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "Sound.h"
#include "StringUtility.h"
#include "TextureManager.h"
#include "WinApp.h"

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif // USE_IMGUI

#include "Camera.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"

#include <DbgHelp.h>
#include <cassert>
#include <strsafe.h>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dinput8.lib")

Matrix4x4 MakeScaleMatrix(const Vector3& scale)
{

    Matrix4x4 result { scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

    return result;
}
Matrix4x4 MakeTranslateMatrix(const Vector3& translate)
{
    Matrix4x4 result { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f };

    return result;
}

// CrashHandler

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* excption)
{
    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t filePath[MAX_PATH] = { 0 };
    CreateDirectory(L"./Dumps", nullptr);
    StringCchPrintf(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
    HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    // processId とクラッシュの発生したthreadidを取得
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();
    // 設定情報を入力
    MINIDUMP_EXCEPTION_INFORMATION minidumpInformation { 0 };
    minidumpInformation.ThreadId = threadId;
    minidumpInformation.ExceptionPointers = excption;
    minidumpInformation.ClientPointers = TRUE;

    // Dumpを出力
    MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);

    return EXCEPTION_EXECUTE_HANDLER;
}

// SoundData SoundLoadWave(const char* filename)
//{
//
//     // ファイル入力ストリームのインスタンス
//     std::ifstream file;
//     // .wavファイルをバイナリモードで開く
//     file.open(filename, std::ios_base::binary);
//     // ファイルオープン失敗を検出する
//     assert(file.is_open());
//
//     // RIFFヘッダーの読み込み
//     RiffHeader riff;
//     file.read((char*)&riff, sizeof(riff));
//     // ファイルがRIFFかチェック
//     if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
//         assert(0);
//     }
//     // タイプがWAVEかチェック
//     if (strncmp(riff.type, "WAVE", 4) != 0) {
//         assert(0);
//     }
//
//     // Formatチャンクの読み込み
//     FormatChunk format = {};
//     // チャンクヘッダーの確認
//     file.read((char*)&format, sizeof(ChunkHeader));
//     if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
//         assert(0);
//     }
//     // チャンク本体の読み込み
//     assert(format.chunk.size <= sizeof(format.fmt));
//     file.read((char*)&format.fmt, format.chunk.size);
//
//     // Dataチャンクの読み込み
//     ChunkHeader data;
//     file.read((char*)&data, sizeof(data));
//     // JUNKチャンクを検出した場合
//     if (strncmp(data.id, "JUNK", 4) == 0) {
//         // 読み込み位置をJUNKチャンクの終わりまで進める
//         file.seekg(data.size, std::ios_base::cur);
//         // 再度読み込み
//         file.read((char*)&data, sizeof(data));
//     }
//
//     if (strncmp(data.id, "data", 4) != 0) {
//         assert(0);
//     }
//
//     // Dataチャンクのデータ部の読み込み
//     char* pBuffer = new char[data.size];
//     file.read(pBuffer, data.size);
//
//     // waveファイルを閉じる
//     file.close();
//
//     // Returnするための音声データ
//     SoundData soundData = {};
//
//     soundData.wfex = format.fmt;
//     soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
//     soundData.bufferSize = data.size;
//
//     return soundData;
// }
//
// void SoundUhload(SoundData* soundData)
//{
//     // バッフアのメモリ解放
//     delete[] soundData->pBuffer;
//
//     soundData->pBuffer = 0;
//     soundData->bufferSize = 0;
//     soundData->wfex = {};
// }
//
// void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
//{
//     HRESULT result;
//
//     // 波形フォーマットを元にSoundVoiceの生成
//     IXAudio2SourceVoice* pSourceVoice = nullptr;
//     result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
//     assert(SUCCEEDED(result));
//
//     // 再生する波形データの設定
//     XAUDIO2_BUFFER buf {};
//     buf.pAudioData = soundData.pBuffer;
//     buf.AudioBytes = soundData.bufferSize;
//     buf.Flags = XAUDIO2_END_OF_STREAM;
//
//     // 波形データの再生
//     result = pSourceVoice->SubmitSourceBuffer(&buf);
//     result = pSourceVoice->Start();
// }

// windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // 誰も捕捉しなかった場合に、捕捉する関数を登録
    SetUnhandledExceptionFilter(ExportDump);

    // winapp初期化
    WinApp* winApp = nullptr;
    winApp = new WinApp();
    winApp->Initialize();

    DirectXCommon* dxCommon = nullptr;
    dxCommon = new DirectXCommon();
    dxCommon->Initialize(winApp);

    Input* input = nullptr;
    input = new Input();
    input->Initialize(winApp);

    SrvManager* srvManager = nullptr;
    srvManager = new SrvManager();
    srvManager->Initialize(dxCommon);

    ImGuiManager* imguiManager = nullptr;
    imguiManager = new ImGuiManager();
    imguiManager->Initialize(winApp, dxCommon, srvManager);

    SpriteCommon* spriteCommon = nullptr;
    spriteCommon = new SpriteCommon;
    spriteCommon->Initialize(dxCommon);

    Object3dCommon* object3dCommon = nullptr;
    object3dCommon = new Object3dCommon();
    object3dCommon->Initialize(dxCommon);

    ModelCommon* modelCommon = nullptr;
    modelCommon = new ModelCommon();
    modelCommon->Initialize(dxCommon);

    Camera* camera = new Camera();
    camera->SetTranslate({ 0.0f, 4.0f, -10.0f });
    camera->SetRotate({ 0.3f, 0.0f, 0.0f });

    TextureManager::getInstance()->Initialize(dxCommon, srvManager);
    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");
    TextureManager::getInstance()->LoadTexture("resources/monsterBall.png");

    ModelManager::GetInstance()->Initialize(dxCommon);
    ModelManager::GetInstance()->LoadModel("Plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");

    ParticleManager::getInstance()->Initialize(dxCommon, srvManager, winApp);
    ParticleManager::getInstance()->SetDefaultCamera(camera);
    ParticleManager::getInstance()->CreateParticleGroup("pori", "resources/circle.png");

    /// ============================================================================================================
    /// 音声データ
    /// ============================================================================================================

    // Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    // IXAudio2MasteringVoice* masterVoice;

    //// xAudioエンジンインスタンスを生成
    // HRESULT resultAudio = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

    // resultAudio = xAudio2->CreateMasteringVoice(&masterVoice);
    // assert(SUCCEEDED(resultAudio));

    //// 音声読み込み
    // SoundData soundData1 = SoundLoadWave("resources/fanfare.wav");

    //// 1度だけ→初期化の後
    // SoundPlayWave(xAudio2.Get(), soundData1);

    Audio audio;
    audio.Initialize();

    Sound fanfare;
    fanfare.Load("resources/fanfare.wav");
    Sound clearSe;
    clearSe.Load("resources/fanfare.wav");

    audio.Play(fanfare);
    audio.Play(clearSe);

    ///
    /// その他
    ///

    bool isSprite = true;
    std::vector<Sprite*> sprites;
    for (uint32_t i = 0; i < 1; ++i) {
        Sprite* sprite = new Sprite();
        if (i % 2 == 0) {
            sprite->Initialize(spriteCommon, winApp, "resources/uvChecker.png");
            sprite->SetPosition(Vector2(100.0f, 100.0f));
        } else {
            sprite->Initialize(spriteCommon, winApp, "resources/monsterBall.png");
        }

        sprites.push_back(sprite);
    }

    object3dCommon->SetDefaultCamera(camera);

    Object3d* object3d2 = new Object3d();
    object3d2->Initialize(object3dCommon, winApp);

    Model* model2 = new Model();
    model2->Initialize(modelCommon, "resources", "plane.obj");
    object3d2->SetModel("axis.obj");

    Object3d* object3d = new Object3d();
    object3d->Initialize(object3dCommon, winApp);

    Model* model = new Model();
    model->Initialize(modelCommon, "resources", "plane.obj");
    object3d->SetModel(model);

    // 板ポリ
    Transform emitter {};
    emitter.translate = { 0.0f, 0.0f, 0.0f };
    emitter.rotate = { 0.0f, 0.0f, 0.0f };
    emitter.scale = { 1.0f, 1.0f, 1.0f };
    ParticleEmitter* particleEmitter = new ParticleEmitter("pori", emitter, 1.0f, uint32_t(3));

    // ウィンドウの×ボタンが押されるまでループ
    while (true) {

        // windowにメッセージが来ていたら最優先で処理させる
        if (winApp->ProcessMessage()) {
            break;
        }

        input->Update();

        if (input->TriggerKey(DIK_0)) {
            OutputDebugStringA("hit 0\n");
            audio.Play(clearSe);
        }

        if (input->PushKey(DIK_1)) {
            OutputDebugStringA("hit 1\n");
        }

        imguiManager->Begin();

        // update/更新処理

        for (uint32_t i = 0; i < sprites.size(); ++i) {
            sprites[i]->Update();
        }

        Vector3 rotate = object3d->GetRotate();
        rotate.x += 0.1f;
        rotate.y += 0.1f;
        object3d->SetRotate(rotate);
        object3d->Update();

        object3d2->Update();
        Vector3 rotate2 = object3d2->GetRotate();
        rotate2.x += -0.1f;
        rotate2.y += -0.1f;
        object3d2->SetRotate(rotate2);

        particleEmitter->Update();

        ParticleManager::getInstance()->Update();

        camera->Update();

#ifdef USE_IMGUI
        ImGui::ShowDemoWindow();
#endif // USE_IMGUI

        imguiManager->End();

        // draw

        dxCommon->PreDraw();

        srvManager->PreDraw();

        object3dCommon->PrepareObjectDraw();

        //
        // 2d/スプライト
        //
        spriteCommon->PrepareSpriteDraw();

        for (uint32_t i = 0; i < sprites.size(); ++i) {
            sprites[i]->Draw();
        }

        // ParticleManager::getInstance()->Draw();

        // object3d->Draw();
        // object3d2->Draw();

        //
        // モデルデータ
        //

        // 実際のcommandListのImGuiの描画コマンドを詰む
        // ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
        imguiManager->Draw();

        dxCommon->PostDraw();
    }

    // ImGuiの終了
    /*ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();*/

    // XAuido2解放
    // xAudio2.Reset();
    // 音声データ解放
    // SoundUhload(&soundData1);
    fanfare.Unload();
    audio.Finalize();

    CloseHandle(dxCommon->GetfenceEvent());
    winApp->Finalize();

    delete input;
    delete winApp;
    TextureManager::getInstance()->Finalize();
    ModelManager::GetInstance()->Finalize();
    ParticleManager::getInstance()->Finalize();
    imguiManager->Finalize();
    delete imguiManager;
    delete srvManager;
    delete dxCommon;
    delete spriteCommon;
    for (uint32_t i = 0; i < sprites.size(); ++i) {
        delete sprites[i];
    }
    delete object3dCommon;
    delete object3d;
    delete object3d2;
    delete modelCommon;
    delete model;
    delete model2;

    return 0;
}
