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
    Framework::GetSrvManager()->PreDraw(); // srv表示
    Framework::GetOffScreenSurface()->PreDraw();
    SceneManager::GetInstance()->Draw(); // メインの描画
    Framework::GetOffScreenSurface()->PostDraw();

    // =============================================
    // ポストエフェクトのバケツリレー(Ping-Pong Buffer)
    // =============================================
    OffscreenSurface* currentSource = Framework::GetOffScreenSurface();
    OffscreenSurface* currentDest = Framework::GetOffScreenSurfaceB();
    auto pp = PostProcess::GetInstance();

    // グレースケール
    if (pp->IsGrayscale()) {
        currentDest->PreDraw(); // Bを描画先に設定
        pp->SetsrvHandle(currentSource->GetSRVHandle()); // Aの画像をセット
        pp->DrawGrayscale(); // フィルタ実行
        currentDest->PostDraw();
        std::swap(currentSource, currentDest); // 読込先と書込先を反転
    }

    // セピア調
    if (pp->IsSepiascale()) {
        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawSepiascale();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    // ヴィネット
    if (pp->IsVignette()) {
        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawVignette();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    // ボックスフィルター 3x3
    if (pp->IsBoxFilter3x3()) {
        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawHorizontalBlur(false); // 引数 false で 3x3
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);

        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawVerticalBlur(false);
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    // ボックスフィルター 5x5
    if (pp->IsBoxFilter5x5()) {
        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawHorizontalBlur(true); // 引数 true で 5x5
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);

        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawVerticalBlur(true);
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    Framework::GetDirectXCommon()->PreDraw();

    // スワップチェーンにコピー!w
    pp->SetsrvHandle(currentSource->GetSRVHandle());
    pp->DrawNormal();

    // 実際のcommandListのImGuiの描画コマンドを詰む
    Framework::GetImGuiManager()->Draw();
    Framework::GetDirectXCommon()->PostDraw();
}

void Game::Finalize()
{
    Framework::Finalize();

    SceneManager::GetInstance()->Finalize();
}