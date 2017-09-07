#include <iostream>
#include <cstring>
#include <chrono>

#include "component.h"
#include "entt/registry.hpp"

#include "net.h"
#include "packet.h"

using ECS = entt::DefaultRegistry<Position, Velocity, Gravity>;
ECS ecs;

void gravitySystem(std::chrono::duration<double> updateRate)
{
        for (auto entity: ecs.view<Velocity, Gravity>())
        {
                auto &velocity = ecs.get<Velocity>(entity);
                auto &gravity = ecs.get<Gravity>(entity);

                velocity.x += gravity.x * updateRate.count();
                velocity.y += gravity.y * updateRate.count();
        }
}

void moveSystem(std::chrono::duration<double> updateRate)
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

class GameServer
{
        using Duration = std::chrono::duration<double>;
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;

public:
        GameServer();

        void update();
        bool isRunning;

private:
        Net net;

        Duration delta, updateDelta, updateRate;

        TimePoint previous, current;
};

GameServer::GameServer()
{
        previous = Clock::now();
        updateRate = Duration(0.0325);

        isRunning = true;

        auto entity = ecs.create();
        ecs.assign<Position>(entity, 0, 0);
        ecs.assign<Velocity>(entity, 0, 0);
        ecs.assign<Gravity>(entity, 0, 100);
}

void GameServer::update()
{
        current = Clock::now();
        delta = current - previous;
        previous = current;

        updateDelta += delta;

        int loops = 0;
        while (updateDelta >= updateRate && loops < 5)
        {
                updateDelta -= updateRate;
                loops++;

                net.handle();

                gravitySystem(updateRate);
                moveSystem(updateRate);
                collisionSystem();

                Packet packet;
                packet.x = 85.2f;
                packet.y = 42.5f;

                net.send(0, packet);
        }

        return;
}

int main()
{
        GameServer server;

        while(server.isRunning)
        {
                server.update();
        }

	return 0;
}
