#ifndef COMPONENT_CONTAINER_H
#define COMPONENT_CONTAINER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>

#include <iostream>

template <class Base>
class ComponentContainerBase
{
      public:
        virtual ~ComponentContainerBase(){};

        uint32_t insert(const Base& x)
        {
                return this->insert_(x);
        }

        Base &get(const uint32_t id)
        {
                return this->get_(id);
        }

        bool contains(const uint32_t id)
        {
                return this->contains_(id);
        }

        std::size_t size()
        {
                return this->size_();
        }

        void remove(const uint32_t id)
        {
                this->remove_(id);
        }

      private:
        virtual uint32_t insert_(const Base &x) = 0;
        virtual void remove_(const uint32_t) = 0;
        virtual char *begin_() = 0;
        virtual const char *begin_() const = 0;
        virtual Base &get_(const uint32_t) = 0;
        virtual bool contains_(const uint32_t) = 0;
        virtual std::size_t size_() const = 0;
        virtual std::size_t element_size_() const = 0;
};

template <class Derived, class Base>
class ComponentContainer : public ComponentContainerBase<Base>
{
      private:
        virtual uint32_t insert_(const Base &x)
        {
                store.push_back(static_cast<const Derived &>(x));

                return store.size() - 1;
        }

        virtual char *begin_()
        {
                return reinterpret_cast<char *>(
                    static_cast<Base *>(const_cast<Derived *>(store.data())));
        }

        virtual const char *begin_() const
        {
                return reinterpret_cast<const char *>(
                    static_cast<const Base *>(store.data()));
        }

        virtual Base &get_(const uint32_t id)
        {
                return static_cast<Base&>(store[id]);
        }

        virtual bool contains_(const uint32_t id)
        {
                if (id > store.size()) {
                        return false;
                }
                return true;
        }

        virtual void remove_(uint32_t id)
        {
                std::swap(store[id], store.back());
                store.pop_back();
                return;
        }

        virtual std::size_t size_() const { return store.size(); }
        virtual std::size_t element_size_() const { return sizeof(Derived); }

        std::vector<Derived> store;
};

#endif // COMPONENT_CONTAINER_H
