#ifndef NET_H
#define NET_H

#include <string>

#include "SDL2/SDL_net.h"

const unsigned short BUFFER_SIZE = 512;

class Net
{
public:
        Net();
        ~Net();

        void connect();
        void send(std::string);
        std::string recv();
private:
        bool connected;

        TCPsocket clientSocket;
        SDLNet_SocketSet socketSet;

	char buffer[BUFFER_SIZE];
};

#endif //NET_H
