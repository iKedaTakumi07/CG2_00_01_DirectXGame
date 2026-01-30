#define DIRECTINPUT_VERSION 0x0800

#include "Framework.h"
#include "BaseScene.h"
#include "Camera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "ModelCommon.h"
#include "Object3dCommon.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include "WinApp.h"

#include "ImGuiManager.h"
#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif // USE_IMGUI

#include <wrl.h>

#include "Audio.h"
#include "ModelManager.h"
#include "ParticleManager.h"
#include "TextureManager.h"
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

void Framework::Initialize()
{
    // 誰も捕捉しなかった場合に、捕捉する関数を登録
    SetUnhandledExceptionFilter(ExportDump);

    // winapp初期化
    /* winApp = new WinApp();
     winApp->Initialize();*/
    WinApp::GetInstance()->Initialize();

    dxCommon = new DirectXCommon();
    dxCommon->Initialize(/*winApp*/);

    input = new Input();
    input->Initialize(/*winApp*/);

    srvManager = new SrvManager();
    srvManager->Initialize(dxCommon);

    imguiManager = new ImGuiManager();
    imguiManager->Initialize(/*winApp,*/ dxCommon, srvManager);

    SpriteCommon::GetInstance()->Initialize(dxCommon);

    Object3dCommon::GetInstance()->Initialize(dxCommon);

    /* modelCommon = new ModelCommon();
     modelCommon->Initialize(dxCommon);*/
    ModelCommon::GetInstance()->Initialize(dxCommon);

    camera = new Camera();
    camera->SetTranslate({ 0.0f, 4.0f, -10.0f });
    camera->SetRotate({ 0.3f, 0.0f, 0.0f });

    Object3dCommon::GetInstance()->SetDefaultCamera(camera);

    TextureManager::getInstance()->Initialize(dxCommon, srvManager);
    ModelManager::GetInstance()->Initialize(dxCommon);
    ParticleManager::getInstance()->Initialize(dxCommon, srvManager /*, winApp*/);
    ParticleManager::getInstance()->SetDefaultCamera(camera);

    baseScene = new BaseScene();

    // audio.Initialize();
    Audio::GetInstance()->Initialize();
}

void Framework::Run()
{
    Initialize();

    while (true) {

        imguiManager->Begin();
        Update();
        imguiManager->End();

        if (IsEndRequst()) {
            break;
        }

        Draw();
    }

    Finalize();
}

void Framework::Update()
{
    if (WinApp::GetInstance()->ProcessMessage()) {
        endRequst_ = true;
    }

    input->Update();

#ifdef USE_IMGUI
    ImGui::ShowDemoWindow();
#endif // USE_IMGUI

    camera->Update();

    ParticleManager::getInstance()->Update();
}

void Framework::Draw()
{
}

void Framework::Finalize()
{

    CloseHandle(dxCommon->GetfenceEvent());
    // winApp->Finalize();

    TextureManager::getInstance()->Finalize();
    ModelManager::GetInstance()->Finalize();
    ParticleManager::getInstance()->Finalize();
    // audio.Finalize();
    delete input;
    // delete winApp;
    imguiManager->Finalize();
    delete imguiManager;
    delete srvManager;
    delete dxCommon;
    // delete modelCommon;
    delete camera;
}
