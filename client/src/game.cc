#include "game.h"

#include <iostream>
#include <random>
#include <cstring>

#include "entity.h"
#include "component.h"

#include "entt/registry.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
                
std::string basePath = SDL_GetBasePath();
std::chrono::duration<double> updateRate;

using ECS = entt::DefaultRegistry<Position, Velocity, Gravity, Render>;
ECS ecs;

Game::Game()
{
        previous = std::chrono::high_resolution_clock::now();
        updateRate = std::chrono::duration<double>(0.0325);

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
                std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        }

        if (SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer))
        {
                std::cout << "SDL_CreateWindowAndRenderer failed: "
                          << SDL_GetError() << std::endl;
        }

        texture = IMG_LoadTexture(renderer, (basePath + std::string("resources/square.png")).c_str());

	net.connect();
	net.send("Hello, World!");

        isRunning = true;
}

Game::~Game()
{
        isRunning = false;

        ecs.reset();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
}

void gravitySystem()
{
        for (auto entity: ecs.view<Velocity, Gravity>())
        {
                auto &velocity = ecs.get<Velocity>(entity);
                auto &gravity = ecs.get<Gravity>(entity);

                velocity.x += gravity.x * updateRate.count();
                velocity.y += gravity.y * updateRate.count();
        }
}

void moveSystem()
{
        for (auto entity: ecs.view<Position, Velocity>())
        {
                auto &position = ecs.get<Position>(entity);
                auto &velocity = ecs.get<Velocity>(entity);

                position.lastX = position.x;
                position.lastY = position.y;

                position.x += velocity.x * updateRate.count();
                position.y += velocity.y * updateRate.count();
        }
}

void collisionSystem()
{
        for (auto entity: ecs.view<Position, Velocity>())
        {
                auto &position = ecs.get<Position>(entity);
                auto &velocity = ecs.get<Velocity>(entity);

                if (position.x < 0)
                {
                        position.x = 0;
                        velocity.x *= -1;
                }
                else if (position.x + 100 > 800)
                {

                        position.x = 800 - 100;
                        velocity.x *= -1;
                }

                if (position.y < 0)
                {
                        position.y = 0;
                        velocity.y = 0;
                }
                else if (position.y + 100 > 600)
                {

                        position.y = 600 - 100;
                        velocity.y *= -1;
                }
        }
}

template <typename T>
T interpolate(T a, T b, double alpha)
{
        return b * alpha + a * (1.0 - alpha);
}

void renderSystem(SDL_Renderer* renderer, double alpha)
{
        for (auto entity: ecs.view<Position, Render>())
        {
                auto &position = ecs.get<Position>(entity);
                auto &render = ecs.get<Render>(entity);

                SDL_Rect texture_rect;

                texture_rect.x = interpolate(position.lastX, position.x, alpha);
                texture_rect.y = interpolate(position.lastY, position.y, alpha);
                texture_rect.w = 100;
                texture_rect.h = 100;

                SDL_RenderCopy(renderer, render.sprite, NULL, &texture_rect);
        }
}

void Game::gameLoop()
{
        current = std::chrono::high_resolution_clock::now();
        delta = current - previous;
        previous = current;

        updateDelta += delta;

        SDL_Event event;

        SDL_PollEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
                isRunning = false;
                break;
        case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                        case SDLK_ESCAPE:
                                isRunning = false;
                                break;
                        default:
                                break;
                }
                break;
        default:
                break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        int loops = 0;
        while (updateDelta >= updateRate && loops < 5)
        {
                updateDelta -= updateRate;
                loops++;

                auto message = net.recv();
                if (message.length() > 0 && message == "Hello, World!")
                {

                        std::random_device rd;
                        std::mt19937 mt(rd());
                        std::uniform_int_distribution<int> dist(0, 800);
                        std::uniform_int_distribution<int> dist2(0, 100);
                        std::uniform_int_distribution<int> dist3(0, 400);
                        std::uniform_int_distribution<int> dist4(100, 200);

                        auto entity = ecs.create();
                        ecs.assign<Position>(entity, dist(mt), dist2(mt));
                        ecs.assign<Velocity>(entity, dist3(mt), 0);
                        ecs.assign<Gravity>(entity, 0, dist4(mt));
                        ecs.assign<Render>(entity, texture);
                }

                gravitySystem();
                moveSystem();
                collisionSystem();
        }

        double alpha = updateDelta / updateRate;

        renderSystem(renderer, alpha);

        SDL_RenderPresent(renderer);

#if __EMSCRIPTEN__
        if (!isRunning())
        {
                emscripten_cancel_main_loop();
        }
#endif

        return;
}
