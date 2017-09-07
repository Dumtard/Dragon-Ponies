#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>
#include <ostream>

#include "SDL2/SDL.h"

class Component
{
      public:
        virtual ~Component(){};

        uint64_t entityId;

        friend std::ostream& operator<<(std::ostream& os, Component& c)
        {
                os << "<Component>()";
                return os;
        }
};

class Position : public Component
{
      public:
        typedef std::shared_ptr<Position> ptr;

        Position() : x(0.f), y(0.f), lastX(0.f), lastY(0.f){};
        Position(int x, int y) : x(x), y(y), lastX(x), lastY(y){};

        double x, y;
        double lastX, lastY;

        friend std::ostream& operator<<(std::ostream& os, Position& p)
        {
                os << "<Position>(" << p.x << ", " << p.y << ")";
                return os;
        }
};

class Velocity : public Component
{
      public:
        typedef std::shared_ptr<Velocity> ptr;

        Velocity() : x(0), y(0){};
        Velocity(int x, int y) : x(x), y(y){};

        double x, y;

        friend std::ostream& operator<<(std::ostream& os, Velocity& v)
        {
                os << "<Velocity>(" << v.x << ", " << v.y << ")";
                return os;
        }
};

class Render : public Component
{
      public:
        typedef std::shared_ptr<Render> ptr;

        Render(SDL_Texture* sprite)
        {
                this->sprite = sprite;
        };

        SDL_Texture* sprite;

        friend std::ostream& operator<<(std::ostream& os, Render& r)
        {
                os << "<Render>()";
                return os;
        }
};

class Gravity : public Component
{
      public:
        typedef std::shared_ptr<Gravity> ptr;

        Gravity() : x(0), y(0){};
        Gravity(int x, int y) : x(x), y(y){};

        double x, y;

        friend std::ostream& operator<<(std::ostream& os, Gravity& g)
        {
                os << "<Gravity>(" << g.x << ", " << g.y << ")";
                return os;
        }
};

#endif // COMPONENT_H
