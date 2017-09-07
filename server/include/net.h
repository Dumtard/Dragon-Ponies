#ifndef NET_H
#define NET_H

#include <string>

#include <SDL2/SDL_net.h>

#include "packet.h"

const unsigned short PORT = 1234;
const unsigned short BUFFER_SIZE = 512;
const unsigned short MAX_SOCKETS = 4;
const unsigned short MAX_CLIENTS = MAX_SOCKETS - 1;

const std::string SERVER_NOT_FULL = "OK";
const std::string SERVER_FULL = "FULL";

class Net
{
public:
	Net();
	~Net();

        void handle();
        void send(int, Packet);

private:
	IPaddress serverIP;
	TCPsocket socket;
	TCPsocket clients[MAX_CLIENTS];
	bool socketIsFree[MAX_CLIENTS];

        SDLNet_SocketSet socketSet;

	char buffer[BUFFER_SIZE];
	int clientCount;

        void checkNewConnection();
        void addNewConnection();
        void rejectNewConnection();
        int getFreeSpot();
        void read(int);
};

#endif //NET_H
