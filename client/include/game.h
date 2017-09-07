#ifndef GAME_H
#define GAME_H

#include <chrono>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "world.h"
#include "net.h"

class Game
{
public:
        Game();
        ~Game();

        bool isRunning;

        void gameLoop();

private:
        std::chrono::duration<double> delta;
        std::chrono::duration<double> updateDelta;

        std::chrono::time_point<std::chrono::high_resolution_clock> previous, current;

        World world;
        Net net;

        SDL_Window *window;
        SDL_Renderer *renderer;

        SDL_Texture* texture;
};

#endif // GAME_H
