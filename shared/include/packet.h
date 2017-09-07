#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include <cstring>

#include <SDL2/SDL_endian.h>

SDL_FORCE_INLINE float swapFloat(float x)
{
        if (SDL_BYTEORDER != SDL_BIG_ENDIAN)
        {
                std::cout << "HERE" << std::endl;
                return SDL_SwapFloat(x);
        }
        return x;
}

struct Packet
{
        float x;
        float y;

public:
        Packet() : x(0), y(0) {}

        Packet(float x, float y)
        {
                this->x = swapFloat(x);
                this->y = swapFloat(y);
        }

        Packet(char* buffer)
        {
                auto end = sizeof(Packet);

                std::copy(buffer, buffer+end, reinterpret_cast<char*>(this));

                this->x = swapFloat(x);
                this->y = swapFloat(y);
        }

        void serialize(char*);

        friend std::ostream& operator<< (std::ostream& os, const Packet& p)
        {
                os << p.x << ", " << p.y;
                return os;
        }
};

inline void Packet::serialize(char* msg)
{
        Packet packet(this->x, this->y);
        std::memcpy(msg, &packet, sizeof(Packet));
}

#endif //PACKET_H
