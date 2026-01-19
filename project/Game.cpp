#include "Game.h"

#include "D3dResourceLeakChecker.h"

#include "ImGuiManager.h"
#include "Input.h"
#include "Logger.h"
#include "Math.h"
#include "Object3dCommon.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "SrvManager.h"

#include "Audio.h"
#include "StringUtility.h"
#include "TextureManager.h"

#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteCommon.h"

#include "GamePlayScene.h"

void Game::Initialize()
{
    // 基底クラスの初期化処理
    Framework::Initialize();

    scene_ = new GamePlayScene();

    scene_->Initialize();
}

void Game::Update()
{
    // update/更新処理
    Framework::Update();
    scene_->Update();

    if (input->TriggerKey(DIK_0)) {
        OutputDebugStringA("hit 0\n");
    }

    if (input->PushKey(DIK_1)) {
        OutputDebugStringA("hit 1\n");
    }
}

void Game::Draw()
{
    // draw

    dxCommon->PreDraw();

    srvManager->PreDraw();

    scene_->Draw();

    // 実際のcommandListのImGuiの描画コマンドを詰む
    imguiManager->Draw();

    dxCommon->PostDraw();
}

void Game::Finalize()
{
    Framework::Finalize();

    scene_->Finalize();
}