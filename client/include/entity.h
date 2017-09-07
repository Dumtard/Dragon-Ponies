#ifndef ENTITY_H
#define ENTITY_H

#include <memory>

#include "componentManager.h"

class Entity
{
      public:
        Entity(std::unique_ptr<ComponentManager> const &componentManager, uint32_t id)
            : id(id), componentManager(componentManager){};
        ~Entity();

        uint32_t id;

        template <typename T, typename... Args>
        void add(Args...);

        template <typename... T>
        bool contains();

        template <typename T>
        void remove();

        template <typename T>
        T& get();

      private:
        std::unique_ptr<ComponentManager> const &componentManager;
};

template <typename T, typename... Args>
inline void Entity::add(Args... args)
{
        this->componentManager->addComponent<T>(this->id, std::forward<Args>(args)...);

        return;
}

template <typename T>
inline void Entity::remove()
{
        this->componentManager->removeComponent<T>(this->id);

        return;
}

template <typename... T>
inline bool Entity::contains()
{
        try
        {
                [](...){ }((this->componentManager->hasComponent<T>(this->id))...);

                return true;
        }
        catch(const std::exception& e)
        {
                return false;
        }
}

template <typename T>
inline T& Entity::get()
{
        return this->componentManager->getComponent<T>(this->id);
}

#endif // ENTITY_H
