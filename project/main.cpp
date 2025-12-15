#define DIRECTINPUT_VERSION 0x0800

#include "D3dResourceLeakChecker.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Logger.h"
#include "Math.h"
#include "Object3dCommon.h"
#include "StringUtility.h"
#include "TextureManager.h"
#include "WinApp.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

#include "Camera.h"
#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"

#include <DbgHelp.h>
#include <cassert>
#include <chrono>
#include <dinput.h>

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include <filesystem>
#include <fstream>
#include <strsafe.h>
#include <vector>
#include <xaudio2.h>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "dinput8.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

SoundData SoundLoadWave(const char* filename)
{

    // ファイル入力ストリームのインスタンス
    std::ifstream file;
    // .wavファイルをバイナリモードで開く
    file.open(filename, std::ios_base::binary);
    // ファイルオープン失敗を検出する
    assert(file.is_open());

    // RIFFヘッダーの読み込み
    RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));
    // ファイルがRIFFかチェック
    if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
        assert(0);
    }
    // タイプがWAVEかチェック
    if (strncmp(riff.type, "WAVE", 4) != 0) {
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

    // Dataチャンクの読み込み
    ChunkHeader data;
    file.read((char*)&data, sizeof(data));
    // JUNKチャンクを検出した場合
    if (strncmp(data.id, "JUNK", 4) == 0) {
        // 読み込み位置をJUNKチャンクの終わりまで進める
        file.seekg(data.size, std::ios_base::cur);
        // 再度読み込み
        file.read((char*)&data, sizeof(data));
    }

    if (strncmp(data.id, "data", 4) != 0) {
        assert(0);
    }

    // Dataチャンクのデータ部の読み込み
    char* pBuffer = new char[data.size];
    file.read(pBuffer, data.size);

    // waveファイルを閉じる
    file.close();

    // Returnするための音声データ
    SoundData soundData = {};

    soundData.wfex = format.fmt;
    soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
    soundData.bufferSize = data.size;

    return soundData;
}

void SoundUhload(SoundData* soundData)
{
    // バッフアのメモリ解放
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}

void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
{
    HRESULT result;

    // 波形フォーマットを元にSoundVoiceの生成
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf {};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    // 波形データの再生
    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}

// windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // ログのディレクトリを用意
    std::filesystem::create_directory("logs");
    // 現在時刻を取得(UTC)
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    // ログファイルの名前にコンマ何秒はいらぬから削る
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    // 日本時間(pcの設定)に変更
    std::chrono::zoned_time localTime { std::chrono::current_zone(), nowSeconds };
    // formatを使って年月日_時分秒に変換
    std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
    // 時刻を使ってファイル名を決定
    std::string logFilePath = std::string("logs/") + dateString + ".log";
    // ファイルを作って書き込み準備
    std::ofstream logStream(logFilePath);

    D3dResourceLeakChecker leakChecl;

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

    SpriteCommon* spriteCommon = nullptr;
    spriteCommon = new SpriteCommon;
    spriteCommon->Initialize(dxCommon);

    Object3dCommon* object3dCommon = nullptr;
    object3dCommon = new Object3dCommon();
    object3dCommon->Initialize(dxCommon);

    ModelCommon* modelCommon = nullptr;
    modelCommon = new ModelCommon();
    modelCommon->Initialize(dxCommon);

    TextureManager::getInstance()->Initialize(dxCommon, srvManager);
    ModelManager::GetInstance()->Initialize(dxCommon);

    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");

    TextureManager::getInstance()->LoadTexture("resources/monsterBall.png");

    ModelManager::GetInstance()->LoadModel("Plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");

    // metaDataを基にSRVの設定
    // D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc {};
    // srvDesc.Format = metadata.format;
    // srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    // srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    //// 2枚目metaDataを基にSRVの設定
    // D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2 {};
    // srvDesc2.Format = metadata2.format;
    // srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    // srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

    //// SRVの生成
    // dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

    // dxCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

    // 頂点場合はびゅーを作成する
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * 6);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView {};
    // リソースの先頭のアドレス使う
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    // 使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
    // 1頂点当たりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    // 頂点リソースにデータを書き込む
    VertexData* vertexData = nullptr;
    // 書き込むためのアドレス獲得
    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    float kWindowWidth = 1280.0f;
    float kWindowHeight = 720.0f;

    // heapの設定
    D3D12_HEAP_PROPERTIES heapProperties {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    // depthStencilTextureをウィンドウのサイズで作成
    /* Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthSetencilTextureResource(device, WinApp::KClientWidth, WinApp::KClientHeight);*/

    /*Transform uvTransformSprite {
           { 1.0f, 1.0f, 1.0f },
           { 0.0f, 0.0f, 0.0f },
           { 0.0f, 0.0f, 0.0f },
    };*/

    //// 平行光源
    // Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightMatrixResourceSprite = dxCommon->CreateBufferResource(sizeof(DirectionalLight));
    //// データを書き込み
    // DirectionalLight* directionalLightDataSprite = nullptr;
    //// アドレスを取得
    // directionalLightMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDataSprite));
    //// 書き込み
    // directionalLightDataSprite->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    // directionalLightDataSprite->direction = { 0.0f, -1.0f, 0.0f };
    // directionalLightDataSprite->intensity = 1.0f;

    // 動かす用のtransform
    /*Transform transformSprite { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };*/

    /// ============================================================================================================
    /// 音声データ
    /// ============================================================================================================

    Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice* masterVoice;

    // xAudioエンジンインスタンスを生成
    HRESULT resultAudio = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

    resultAudio = xAudio2->CreateMasteringVoice(&masterVoice);
    assert(SUCCEEDED(resultAudio));

    // 音声読み込み
    SoundData soundData1 = SoundLoadWave("resources/fanfare.wav");

    // 1度だけ→初期化の後
    /*SoundPlayWave(xAudio2.Get(), soundData1);*/

    ///
    /// その他
    ///

    bool isSprite = true;
    std::vector<Sprite*> sprites;
    for (uint32_t i = 0; i < 5; ++i) {
        Sprite* sprite = new Sprite();
        if (i % 2 == 0) {
            sprite->Initialize(spriteCommon, winApp, "resources/uvChecker.png");
        } else {
            sprite->Initialize(spriteCommon, winApp, "resources/monsterBall.png");
        }

        sprites.push_back(sprite);
    }

    Camera* camera = new Camera();
    camera->SetTranslate({ 0.0f, 4.0f, -10.0f });
    camera->SetRotate({ 0.3f, 0.0f, 0.0f });
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
    ParticleManager* particleManager = new ParticleManager();

    // ウィンドウの×ボタンが押されるまでループ
    while (true) {

        // windowにメッセージが来ていたら最優先で処理させる
        if (winApp->ProcessMessage()) {
            break;
        }

        input->Update();

        if (input->TriggerKey(DIK_0)) {
            OutputDebugStringA("hit 0\n");
        }

        if (input->PushKey(DIK_1)) {
            OutputDebugStringA("hit 1\n");
        }

        // ImGui_ImplDX12_NewFrame();
        // ImGui_ImplWin32_NewFrame();
        // ImGui::NewFrame();

        // ImGui::Begin("Settings");

        // ImGui::End();

        // update/更新処理

        // imguiのUI
        /* ImGui::ShowDemoWindow();*/

        for (uint32_t i = 0; i < sprites.size(); ++i) {
            Vector2 pos = sprites[i]->GetPosition();
            pos.x = i * 100.0f;
            pos.y = i * 100.0f;
            sprites[i]->SetPosition(pos);
            sprites[i]->SetAnchorPoint({ 0.5f, 0.5f });

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

        camera->Update();

        // draw
        // ImGui::Render();

        dxCommon->PreDraw();

        srvManager->PreDraw();

        object3dCommon->PrepareObjectDraw();

        //
        // 2d/スプライト
        //

        object3d->Draw();
        object3d2->Draw();

        spriteCommon->PrepareSpriteDraw();

        for (uint32_t i = 0; i < sprites.size(); ++i) {
            sprites[i]->Draw();
        }

        //
        // モデルデータ
        //

        // 実際のcommandListのImGuiの描画コマンドを詰む
        // ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());

        dxCommon->PostDraw();
    }

    // ImGuiの終了
    /*ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();*/

    // XAuido2解放
    xAudio2.Reset();
    // 音声データ解放
    SoundUhload(&soundData1);

    CloseHandle(dxCommon->GetfenceEvent());
    winApp->Finalize();

    delete input;
    delete winApp;
    TextureManager::getInstance()->Finalize();
    ModelManager::GetInstance()->Finalize();
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
