#ifndef ENTITY_MANAGERS_H
#define ENTITY_MANAGERS_H

#include <memory>
#include <vector>

#include "componentManager.h"
#include "entity.h"

class EntityManager
{
public:
        EntityManager(std::unique_ptr<ComponentManager> const &componentManager) : componentManager(componentManager){};

        static uint32_t id;

        std::vector<int> list;
        std::vector<Entity> entities;

        Entity& createEntity();

private:
        std::unique_ptr<ComponentManager> const &componentManager;
};

#endif // ENTITY_MANAGER_H
