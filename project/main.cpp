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

#include "Sprite.h"
#include "SpriteCommon.h"
#include "Object3d.h"

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

    SpriteCommon* spriteCommon = nullptr;
    spriteCommon = new SpriteCommon;
    spriteCommon->Initialize(dxCommon);

    Object3dCommon* object3dCommon = nullptr;
    object3dCommon = new Object3dCommon();
    object3dCommon->Initialize(dxCommon);

    TextureManager::getInstance()->Initialize(dxCommon);

    // SRVを作成するdescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);

    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxCommon->GetSRVCPUDescriptorHandle(2);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxCommon->GetSRVGPUDescriptorHandle(2);

    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU3 = dxCommon->GetSRVCPUDescriptorHandle(3);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU3 = dxCommon->GetSRVGPUDescriptorHandle(3);

    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU4 = dxCommon->GetSRVCPUDescriptorHandle(4);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU4 = dxCommon->GetSRVGPUDescriptorHandle(4);

    // Textureを読み込み
    // DirectX::ScratchImage mipImages =
    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");
    // const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    // Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata);
    // Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCommon->UploadTextureData(textureResource, mipImages);

    // 2枚目Textureを読み込み
    // DirectX::ScratchImage mipImages2 =
    TextureManager::getInstance()->LoadTexture("resources/monsterBall.png");
    // const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
    // Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata2);
    // Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 = dxCommon->UploadTextureData(textureResource2, mipImages2);

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

    /// ==============================================================================================================
    /// モデルデータ
    /// ==============================================================================================================

    // モデル読み込み
    // ModelData model = LoadObjFile("resources", "axis.obj");

    //// 画像読み込み
    // DirectX::ScratchImage mip2 = (model.material.textureFilePath);
    // const DirectX::TexMetadata& metadata3 = mip2.GetMetadata();
    // Microsoft::WRL::ComPtr<ID3D12Resource> textureResource3 = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata3);
    // Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource3 = dxCommon->UploadTextureData(textureResource3, mip2);

    //// 3枚目metaDataを基にSRVの設定
    // D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc3 {};
    // srvDesc3.Format = metadata3.format;
    // srvDesc3.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // srvDesc3.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    // srvDesc3.Texture2D.MipLevels = UINT(metadata3.mipLevels);

    // dxCommon->GetDevice()->CreateShaderResourceView(textureResource3.Get(), &srvDesc3, textureSrvHandleCPU3);

    // 頂点リソースを作成
    /*   Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceModel = dxCommon->CreateBufferResource(sizeof(VertexData) * model.vertices.size());*/

    // 頂点バッファビューを作成
    // D3D12_VERTEX_BUFFER_VIEW vertexBufferViewModel {};
    // vertexBufferViewModel.BufferLocation = vertexResourceModel->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使用
    // vertexBufferViewModel.SizeInBytes = UINT(sizeof(VertexData) * model.vertices.size()); // 使用するリソースのサイズ
    // vertexBufferViewModel.StrideInBytes = sizeof(VertexData); // 1頂点当たりのサイズ

    // 頂点リソースに書き込み
    // VertexData* vertexDataModel = nullptr;
    // vertexResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataModel)); // 書き込むためのアドレス取得
    // std::memcpy(vertexDataModel, model.vertices.data(), sizeof(VertexData) * model.vertices.size()); // 頂点データをリソースにコピー

    // インデックスリソースにデータを書き込む
    /*Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceModel = dxCommon->CreateBufferResource(sizeof(uint32_t) * model.vertices.size());*/

    // D3D12_INDEX_BUFFER_VIEW indexBufferViewModel {};
    //// リソースの先頭のアドレスから使う
    // indexBufferViewModel.BufferLocation = indexResourceModel->GetGPUVirtualAddress();
    //// 使用するリソースのサイズはインデックス6つ分のサイズ
    // indexBufferViewModel.SizeInBytes = UINT(sizeof(uint32_t) * model.vertices.size());
    //// インデックスはuint32_Tとする
    // indexBufferViewModel.Format = DXGI_FORMAT_R32_UINT;

    // uint32_t* indexDataModel = nullptr;
    // indexResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&indexDataModel));

    //// sphere用のマテリアルリソースを作る
    // Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceModel = dxCommon->CreateBufferResource(sizeof(Material));

    // Material* materialDataModel = nullptr;

    //// mapして書き込み
    // materialResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&materialDataModel));
    //// 今回は白を書き込んでみる
    // materialDataModel->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    // materialDataModel->enableLighting = true;

    // materialDataModel->uvTransform = MakeIdentity4x4();

    //// sphere用のtransformmatrix用のリソースを作る
    // Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceModel = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));
    //// データを書き込む
    // TransformationMatrix* transformationMatrixDataModel = nullptr;
    //// 書き込むためのアドレス取得
    // transformationMatrixResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataModel));
    //// 単位行列を書き込む
    // transformationMatrixDataModel->WVP = MakeIdentity4x4();
    // transformationMatrixDataModel->world = MakeIdentity4x4();

    // Transform transformModel { { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

    //// 平行光源
    // Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightMatrixResourceModel = dxCommon->CreateBufferResource(sizeof(DirectionalLight));
    //// データを書き込み
    // DirectionalLight* directionalLightDataModel = nullptr;
    //// アドレスを取得
    // directionalLightMatrixResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDataModel));
    //// 書き込み
    // directionalLightDataModel->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    // directionalLightDataModel->direction = { 0.0f, -1.0f, 0.0f };
    // directionalLightDataModel->intensity = 1.0f;

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

    Object3d* object3d = new Object3d();
    object3d->Initialize(object3dCommon,winApp);

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

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Settings");

        for (uint32_t i = 0; i < sprites.size(); ++i) {
            ImGui::Text("textureIndex[%u]: %u", i, sprites[i]->GettextureIndex());
        }

        /*ImGui::Checkbox("isSprite", &isSprite);*/

        /* if (ImGui::CollapsingHeader("Model##Model")) {
             ImGui::DragFloat3("Translate##Model", &transformModel.translate.x, 0.01f);
             ImGui::SliderAngle("RotateX##Model", &transformModel.rotate.x);
             ImGui::SliderAngle("RotateY##Model", &transformModel.rotate.y);
             ImGui::SliderAngle("RotateZ##Model", &transformModel.rotate.z);
             ImGui::ColorEdit4("Color##Model", &(materialDataModel->color).x);
             ImGui::SliderFloat3("direction##ModelLight", &directionalLightDataModel->direction.x, -1.0f, 1.0f);
             ImGui::DragFloat("intensity##ModelLight", &directionalLightDataModel->intensity, 0.01f);
             ImGui::ColorEdit4("Color##ModelLight", &(directionalLightDataModel->color).x);
         }*/

        ImGui::End();

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

        object3d->Update();

        /*Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
        uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
        uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
        materialDataSprite->uvTransform = uvTransformMatrix;

        directionalLightDataSprite->direction = Normalize(directionalLightDataSprite->direction);*/

        // 球体

        // モデルデータ

        /*   Matrix4x4 worldMatrixModel = MakeAffineMatrix(transformModel.scale, transformModel.rotate, transformModel.translate);
           Matrix4x4 projectionMatrixModel = MakePrespectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
           Matrix4x4 worldViewProjectionMatrixModel = Multiply(worldMatrixModel, Multiply(viewMatrix, projectionMatrixModel));
           transformationMatrixDataModel->WVP = worldViewProjectionMatrixModel;
           transformationMatrixDataModel->world = worldMatrixModel;

           directionalLightDataModel->direction = Normalize(directionalLightDataModel->direction);*/

        // draw
        ImGui::Render();

        dxCommon->PreDraw();

        object3dCommon->PrepareObjectDraw();

        spriteCommon->PrepareSpriteDraw();

        //
        // 三角形
        //

        // dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
        //// 形状を設定
        //// マテリアルCBuffrtの場所を設定
        // dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
        //// wvp用のCBufferの場所を設定
        // dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
        // dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightMatrixResource->GetGPUVirtualAddress());
        //// SRV
        // dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
        //  描画
        /*dxCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);*/

        //
        // 2d/スプライト
        //

        /*for (uint32_t i = 0; i < sprites.size(); ++i) {
            sprites[i]->Draw();
        }*/

        object3d->Draw();

        //
        // モデルデータ
        //

       

        // 実際のcommandListのImGuiの描画コマンドを詰む
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());

        dxCommon->PostDraw();
    }

    // ImGuiの終了
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // XAuido2解放
    xAudio2.Reset();
    // 音声データ解放
    SoundUhload(&soundData1);

    CloseHandle(dxCommon->GetfenceEvent());
    winApp->Finalize();

    delete input;
    delete winApp;
    TextureManager::getInstance()->Finalize();
    delete dxCommon;
    delete spriteCommon;
    for (uint32_t i = 0; i < sprites.size(); ++i) {
        delete sprites[i];
    }
    delete object3dCommon;
    delete object3d;

    return 0;
}
