#include "Framework.h"
#include "Game.h"
#include <wrl.h>

// windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Framework* game = new Game();

    game->Run();

    delete game;

    return 0;
}
