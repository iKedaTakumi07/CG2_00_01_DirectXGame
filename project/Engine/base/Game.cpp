#include "Game.h"

#include "D3dResourceLeakChecker.h"

#include "../3d/Object3dCommon.h"
#include "../3d/ParticleEmitter.h"
#include "../3d/ParticleManager.h"
#include "../io/Input.h"
#include "ImGuiManager.h"
#include "Logger.h"
#include "Math.h"
#include "SrvManager.h"

#include "../audio/Audio.h"
#include "StringUtility.h"
#include "TextureManager.h"

#include "../2d/Sprite.h"
#include "../2d/SpriteCommon.h"
#include "../3d/Model.h"
#include "../3d/ModelCommon.h"
#include "../3d/ModelManager.h"
#include "../3d/Object3d.h"

#include "../scene/BaseScene.h"
#include "../scene/GamePlayScene.h"
#include "../scene/SceneManager.h"
#include "../scene/TitleScene.h"

void Game::Initialize()
{
    // 基底クラスの初期化処理
    Framework::Initialize();

    auto title = std::make_unique<TitleScene>();
    title->SetInput(Framework::GetInput());
    title->SetCamera(Framework::GetCamera());
    SceneManager::GetInstance()->SetNextScene(std::move(title));
}

void Game::Update()
{
    // update/更新処理
    Framework::Update();

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