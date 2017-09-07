#include "entityManager.h"

uint32_t EntityManager::id = 0;

Entity& EntityManager::createEntity()
{
        Entity e(componentManager, EntityManager::id++);
        this->entities.push_back(e);
        return this->entities.back();
}
