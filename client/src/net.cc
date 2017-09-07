#include "net.h"

#include <iostream>
#include <cstring>
#include <algorithm>

#include "packet.h"

Net::Net()
{
        if (SDLNet_Init() < 0)
        {
                std::cout << "Failed to initialize SDL_net: "
                        << SDLNet_GetError() << std::endl;
        }
}

Net::~Net()
{
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
}

void Net::connect()
{
        connected = false;

	const char *host;
        IPaddress serverIP;

        const unsigned short PORT = 1234;
        std::string serverName = "localhost";


        socketSet = SDLNet_AllocSocketSet(1);
        if (socketSet == NULL)
        {
                std::cout << "Failed to allocate the socket set: "
                        << SDLNet_GetError() << std::endl;
                return;
        }

        int hostResolved = SDLNet_ResolveHost(&serverIP, serverName.c_str(), PORT);

        if (hostResolved == -1)
        {
                std::cout << "Failed to resolve the server hostname: "
                        << SDLNet_GetError() << std::endl;
                return;
        }

	Uint8 * dotQuad = (Uint8*)&serverIP.host;

	std::cout << "Successfully resolved host to IP: "
		<< (unsigned short)dotQuad[0] << "."
		<< (unsigned short)dotQuad[1] << "."
		<< (unsigned short)dotQuad[2] << "."
		<< (unsigned short)dotQuad[3]
		<< " port " << SDLNet_Read16(&serverIP.port)
		<< std::endl;

	// Try to resolve the IP of the server, just for kicks
	if ((host = SDLNet_ResolveIP(&serverIP)) == NULL)
	{
		std::cout << "Failed to resolve the server IP address: "
		        << SDLNet_GetError() << std::endl;
		return;
	}
	std::cout << "Successfully resolved IP to host: " << host << std::endl;

	// Try to open a connection to the server and quit out if we can't connect
	clientSocket = SDLNet_TCP_Open(&serverIP);
	if (!clientSocket)
	{
		std::cout << "Failed to open socket to server: "
		        << SDLNet_GetError() << std::endl;
		return;
	}

	std::cout << "Connection okay, about to read connection status from the server..." << std::endl;

	// Add our socket to the socket set for polling
	SDLNet_TCP_AddSocket(socketSet, clientSocket);

	int active = SDLNet_CheckSockets(socketSet, 5000);
	if (!active)
	{
		return;
	}

	// Check if we got a response from the server
	int ready = SDLNet_SocketReady(clientSocket);
	if (ready)
	{
		std::cout << "Got a response from the server... " << std::endl;
		int bytes = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);
                if (bytes <= 0)
                {
                        return;
                }

		// We got an okay from the server, so we can join!
		if ( std::strcmp(buffer, "OK") == 0 )
		{
			std::cout << "Joining server now..." << std::endl;
			connected = true;
		}
		else
		{
			std::cout << "Server is full... Terminating connection."
			        << std::endl;
		}
	}
	else
	{
		std::cout << "No response from server..." << std::endl;
	}
}

std::string Net::recv()
{
        int socketActive = SDLNet_CheckSockets(socketSet, 0);
        if (!socketActive)
        {
                return "";
        }

        int socketReady = SDLNet_SocketReady(clientSocket);
        if (!socketReady)
        {
                return "";
        }

        int bytes = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);
        if (bytes <= 0)
        {
                return "";
        }

        for (unsigned int i = 0; i < bytes / sizeof(Packet); i++)
        {
                auto start = 0 * sizeof(Packet);

                Packet msg{buffer+start};

                std::cout << i << ". " << "(" << sizeof(Packet) << ") "
                        << msg << std::endl;
        }

        return std::string(buffer, bytes);
}

void Net::send(std::string message)
{
        if (!connected)
        {
                return;
        }
        SDLNet_TCP_Send(clientSocket, message.c_str(), message.length());
}
