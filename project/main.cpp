#include "Game.h"
#include <wrl.h>

Game game;

// windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    game.Initialize();

    // ウィンドウの×ボタンが押されるまでループ
    while (true) {
        game.Update();

        game.Draw();
    }

    game.Finalize();

    return 0;
}
