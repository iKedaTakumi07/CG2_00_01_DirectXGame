#include "Framework.h"
#include "Game.h"
#include <memory>
#include <wrl.h>

// windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    std::unique_ptr<Framework> game;

    game = std::make_unique<Framework>();

    game->Run();

    return 0;
}
