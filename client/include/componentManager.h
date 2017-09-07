#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <unordered_map>
#include <map>
#include <typeindex>
#include <iostream>
#include <memory>
#include <vector>

#include "componentContainer.h"
#include "component.h"

class ComponentManager
{
public:
        template <typename T, typename... Args>
        void addComponent(uint32_t, Args...);

        template <typename T>
        T& getComponent(uint32_t);

        template <typename T>
        bool hasComponent(uint32_t);

        template <typename T>
        void removeComponent(uint32_t);

private:
        typedef ComponentContainerBase<Component> segment;
        typedef std::unique_ptr<segment> ptr;

        std::map<std::type_index, ptr> components;
        std::map<std::type_index, std::vector<uint32_t>> entityComponentPosition;
};

template <typename T, typename... Args>
inline void ComponentManager::addComponent(uint32_t id, Args... args)
{
        std::type_index index = std::type_index(typeid(T));

        if (this->components.find(index) == this->components.end())
        {
                typedef ComponentContainer<T, Component> container;

                this->components[index] = std::make_unique<container>();
        }


        if (this->entityComponentPosition.find(index) == this->entityComponentPosition.end())
        {
                this->entityComponentPosition[index].resize(id+1);
        }

        if (this->entityComponentPosition[index].size() >= id)
        {
                this->entityComponentPosition[index].resize(id+1);
        }

        T component = T(std::forward<Args>(args)...);
        component.entityId = id;

        uint32_t position = this->components[index]->insert(component);
        this->entityComponentPosition[index][id] = position + 1;

        return;
}

template <typename T>
inline T& ComponentManager::getComponent(uint32_t id)
{
        std::type_index index = std::type_index(typeid(T));
        uint32_t componentPosition = this->entityComponentPosition[index][id];

        return dynamic_cast<T&>(this->components[index]->get(componentPosition - 1));
}

template <typename T>
inline bool ComponentManager::hasComponent(uint32_t id)
{
        try
        {
                std::type_index index = std::type_index(typeid(T));
                if (this->entityComponentPosition.find(index) == this->entityComponentPosition.end())
                {
                        throw std::invalid_argument("HERE");
                        return false;
                }
                if (this->entityComponentPosition[index][id] == 0)
                {
                        throw std::invalid_argument("HERE");
                        return false;
                }
                return true;
        }
        catch(const std::exception& e)
        {
                throw std::invalid_argument("HERE");
                return false;
        }
}

template <typename T>
inline void ComponentManager::removeComponent(uint32_t id)
{
        std::type_index index = std::type_index(typeid(T));
        this->components[index]->remove(id);
}

#endif // COMPONENT_MANAGER_H
