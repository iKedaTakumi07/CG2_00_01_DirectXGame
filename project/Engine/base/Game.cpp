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
    PostProcess::GetInstance()->DrawImGui();
}

void Game::Draw()
{
    // draw

    // srv表示
    Framework::GetSrvManager()->PreDraw();

    // オフスクリーン描画
    Framework::GetOffScreenSurface()->PreDraw();

    SceneManager::GetInstance()->Draw();

    Framework::GetOffScreenSurface()->PostDraw();

    // フィルタを使っているかどうか
    PostProcess::Mode currentMode = PostProcess::GetInstance()->GetMode();
    if (currentMode == PostProcess::Mode::kBoxFilterSeparable3x3 || currentMode == PostProcess::Mode::kBoxFilterSeparable5x5) {
        // 横ブラー
        Framework::GetOffScreenSurfaceB()->PreDraw(); // Bを描画先に設定
        PostProcess::GetInstance()->SetsrvHandle(Framework::GetOffScreenSurface()->GetSRVHandle()); // Aの画像をセット
        PostProcess::GetInstance()->DrawHorizontalBlur(); // 横ブラー実行
        Framework::GetOffScreenSurfaceB()->PostDraw();

        // 縦ブラー
        Framework::GetDirectXCommon()->PreDraw(); // 画面を描画先に設定
        PostProcess::GetInstance()->SetsrvHandle(Framework::GetOffScreenSurfaceB()->GetSRVHandle()); // Bの画像をセット
        PostProcess::GetInstance()->DrawVerticalBlur(); // 縦ブラー実行

    } else {
        // 今まで通り
        // スワップチェーンの描画
        Framework::GetDirectXCommon()->PreDraw();
        PostProcess::GetInstance()->SetsrvHandle(Framework::GetOffScreenSurface()->GetSRVHandle());
        PostProcess::GetInstance()->PrepareObjectDraw();
    }

    // 実際のcommandListのImGuiの描画コマンドを詰む
    Framework::GetImGuiManager()->Draw();

    Framework::GetDirectXCommon()->PostDraw();
}

void Game::Finalize()
{
    Framework::Finalize();

    SceneManager::GetInstance()->Finalize();
}