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

#include "BaseScene.h"
#include "GamePlayScene.h"
#include "SceneManager.h"
#include "TitleScene.h"

void Game::Initialize()
{
    // 基底クラスの初期化処理
    Framework::Initialize();

    BaseScene* scene = new TitleScene();
    scene->SetInput(Framework::GetInput());

    SceneManager::GetInstance()->SetNextScene(scene);
    // sceneManager_->SetNextScene(baseScene);
}

void Game::Update()
{
    // update/更新処理
    Framework::Update();
    input_ = Framework::GetInput();

    SceneManager::GetInstance()->Update();
}

void Game::Draw()
{
    // draw

    Framework::GetDirectXCommon()->PreDraw();

    Framework::GetSrvManager()->PreDraw();

    SceneManager::GetInstance()->Draw();

    // 実際のcommandListのImGuiの描画コマンドを詰む
    Framework::GetImGuiManager()->Draw();

    Framework::GetDirectXCommon()->PostDraw();
}

void Game::Finalize()
{
    Framework::Finalize();

    SceneManager::GetInstance()->Finalize();
}