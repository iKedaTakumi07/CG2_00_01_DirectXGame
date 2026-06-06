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
#include "../scene/SceneFactory.h"
#include "../scene/SceneManager.h"

void Game::Initialize()
{
    // 基底クラスの初期化処理
    Framework::Initialize();

    SceneManager::GetInstance()->ChangeScene("TITLE");
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
    Framework::GetOffScreenSurface()->PreDraw(true);
    SceneManager::GetInstance()->Draw(); // メインの描画
    Framework::GetOffScreenSurface()->PostDraw();

    Framework::GetOffScreenSurface()->TransitionDepthToShaderResource();

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

    // ボックスフィルター
    if (pp->IsBoxFilter()) {
        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawBoxFilterHorizontal();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);

        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawBoxFilterVertical();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    // ガウシアンフィルター
    if (pp->IsGaussianFilter()) {
        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawGaussianFilterHorizontal();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);

        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawGaussianFilterVertical();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    // アウトラインフィルタ(輝度)
    if (pp->IsLuminanceOutLine()) {
        currentDest->PreDraw();
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawLuminanceOutLine();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    // アウトラインフィルタ(depth)
    if (pp->IstDepthOutLine()) {
        currentDest->PreDraw();
        pp->SetDepthSrvHandle(currentSource->GetDepthSRVHandle());
        pp->SetsrvHandle(currentSource->GetSRVHandle());
        pp->DrawDepthOutLine();
        currentDest->PostDraw();
        std::swap(currentSource, currentDest);
    }

    Framework::GetOffScreenSurface()->TransitionDepthToWritable();

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