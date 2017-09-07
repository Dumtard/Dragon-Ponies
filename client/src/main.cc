#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "game.h"

void mainLoop(void *g)
{
        Game *game = (Game *)g;
        game->gameLoop();
}

int main(int argc, char *argv[])
{
        Game game;

#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(mainLoop, &game, 0, 1);
#else
        while (game.isRunning)
        {
                mainLoop(&game);
        }
#endif

        return 0;
}
