#include "world.h"

World::World()
{
        this->componentManager = std::make_unique<ComponentManager>();
        this->entityManager = std::make_unique<EntityManager>(this->componentManager);
}

Entity& World::createEntity()
{
        return this->entityManager->createEntity();
}
