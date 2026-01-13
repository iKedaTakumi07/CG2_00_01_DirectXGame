#include "Game.h"

#define DIRECTINPUT_VERSION 0x0800


#include "D3dResourceLeakChecker.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Logger.h"
#include "Math.h"
#include "Object3dCommon.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"

#include "StringUtility.h"
#include "TextureManager.h"


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
#include <strsafe.h>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxcompiler.lib")

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

void Game::Initialize()
{
    // 誰も捕捉しなかった場合に、捕捉する関数を登録
    SetUnhandledExceptionFilter(ExportDump);

    // winapp初期化
    winApp = new WinApp();
    winApp->Initialize();

    dxCommon = new DirectXCommon();
    dxCommon->Initialize(winApp);

    input = new Input();
    input->Initialize(winApp);

    srvManager = new SrvManager();
    srvManager->Initialize(dxCommon);

    imguiManager = new ImGuiManager();
    imguiManager->Initialize(winApp, dxCommon, srvManager);

    spriteCommon = new SpriteCommon;
    spriteCommon->Initialize(dxCommon);

    object3dCommon = new Object3dCommon();
    object3dCommon->Initialize(dxCommon);

    modelCommon = new ModelCommon();
    modelCommon->Initialize(dxCommon);

    camera = new Camera();
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

    audio.Initialize();

    fanfare.SoundLoadFile("resources/fanfare.wav");

    clearSe.SoundLoadFile("resources/stage.mp3");

    audio.Play(fanfare);
    audio.Play(clearSe);

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

    object3d2 = new Object3d();
    object3d2->Initialize(object3dCommon, winApp);

    model2 = new Model();
    model2->Initialize(modelCommon, "resources", "plane.obj");
    object3d2->SetModel("axis.obj");

    object3d = new Object3d();
    object3d->Initialize(object3dCommon, winApp);

    model = new Model();
    model->Initialize(modelCommon, "resources", "plane.obj");
    object3d->SetModel(model);

    // 板ポリ
    Transform emitter {};
    emitter.translate = { 0.0f, 0.0f, 0.0f };
    emitter.rotate = { 0.0f, 0.0f, 0.0f };
    emitter.scale = { 1.0f, 1.0f, 1.0f };
    particleEmitter = new ParticleEmitter("pori", emitter, 1.0f, uint32_t(3));
}

void Game::Update()
{
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
}

void Game::Draw()
{

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

void Game::Finalize()
{
    // ImGuiの終了
    /*ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();*/

    // XAuido2解放
    // xAudio2.Reset();
    // 音声データ解放
    // SoundUhload(&soundData1);

    fanfare.Unload();
    clearSe.Unload();
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
}