#ifndef WORLD_H
#define WORLD_H

#include <memory>

#include "entity.h"
#include "entityManager.h"
#include "componentManager.h"

class World
{
public:
        World();
        Entity& createEntity();

        std::unique_ptr<EntityManager> entityManager;
        std::unique_ptr<ComponentManager> componentManager;
};

#endif // WORLD_H
