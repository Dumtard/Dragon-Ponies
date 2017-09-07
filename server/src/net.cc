#include "net.h"

Net::Net()
{
	clientCount = 0;

	if (SDLNet_Init() == -1)
	{
		std::cout << "Failed to intialise SDL_net: "
		        << SDLNet_GetError() << std::endl;
		return;
	}

	socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
	if (socketSet == NULL)
	{
		std::cout << "Failed to allocate the socket set: "
		        << SDLNet_GetError() << std::endl;
		return;
	}

	std::cout << "Allocated socket set with size:  " << MAX_SOCKETS
	        << ", of which " << MAX_CLIENTS
	        << " are availble for use by clients." << std::endl;

	// Initialize all the client sockets (i.e. blank them ready for use!)
	for (int loop = 0; loop < MAX_CLIENTS; loop++)
	{
		clients[loop] = NULL;
		socketIsFree[loop] = true;
	}

	int hostResolved = SDLNet_ResolveHost(&serverIP, NULL, PORT);
	if (hostResolved == -1)
	{
		std::cout << "Failed to resolve the server host: "
		        << SDLNet_GetError() << std::endl;
		return;
	}
	Uint8 * dotQuad = (Uint8*)&serverIP.host;

	std::cout << "Successfully resolved server host to IP: "
	        << (unsigned short)dotQuad[0] << "."
	        << (unsigned short)dotQuad[1] << "."
	        << (unsigned short)dotQuad[2] << "."
	        << (unsigned short)dotQuad[3] << ":"
	        << SDLNet_Read16(&serverIP.port) << std::endl;

	// Try to open the server socket
	socket = SDLNet_TCP_Open(&serverIP);
	if (!socket)
	{
		std::cout << "Failed to open the server socket: "
		        << SDLNet_GetError() << std::endl;
		return;
	}
	std::cout << "Sucessfully created server socket." << std::endl;

	SDLNet_TCP_AddSocket(socketSet, socket);

	std::cout << "Awaiting clients..." << std::endl;
}

Net::~Net()
{
	SDLNet_FreeSocketSet(socketSet);
	SDLNet_TCP_Close(socket);
	SDLNet_Quit();
}

void Net::handle()
{
	int activeSockets = SDLNet_CheckSockets(socketSet, 0);
	if (!activeSockets)
	{
		return;
	}

	checkNewConnection();

	for (int clientNumber = 0; clientNumber < MAX_CLIENTS; clientNumber++)
	{
		read(clientNumber);
	}
}

int Net::getFreeSpot()
{
	int freeSpot = -99;
	for (int loop = 0; loop < MAX_CLIENTS; loop++)
	{
		if (socketIsFree[loop] == true)
		{
			socketIsFree[loop] = false;
			freeSpot = loop;
			break;
		}
	}

	return freeSpot;
}

void Net::addNewConnection()
{
        int freeSpot = getFreeSpot();
        if (freeSpot < 0)
        {
                rejectNewConnection();
                return;
        }

	clients[freeSpot] = SDLNet_TCP_Accept(socket);
	SDLNet_TCP_AddSocket(socketSet, clients[freeSpot]);
	clientCount++;

	// Send a message to the client saying "OK" to indicate the incoming
	// connection has been accepted
	std::strcpy( buffer, SERVER_NOT_FULL.c_str() );
	int msgLength = std::strlen(buffer) + 1;
	SDLNet_TCP_Send(clients[freeSpot], (void *)buffer, msgLength);

	std::cout << "Client connected. There are now " << clientCount
	        << " client(s) connected." << std::endl;
}

void Net::rejectNewConnection()
{
	std::cout << "*** Maximum client count reached - "
	        << "rejecting client connection ***" << std::endl;

	TCPsocket tempSock = SDLNet_TCP_Accept(socket);

	std::strcpy( buffer, SERVER_FULL.c_str() );
	int msgLength = std::strlen(buffer) + 1;
	SDLNet_TCP_Send(tempSock, (void *)buffer, msgLength);

	// Shutdown, disconnect, and close the socket to the client
	SDLNet_TCP_Close(tempSock);
}

void Net::checkNewConnection()
{
	int socketReady = SDLNet_SocketReady(socket);
	if (!socketReady)
	{
	        return;
	}

	if (clientCount < MAX_CLIENTS)
	{
	        addNewConnection();
	}
	else
	{
	        rejectNewConnection();
	}
}

void Net::read(int client)
{
	int socketReady = SDLNet_SocketReady(clients[client]);
	if (!socketReady)
	{
	        return;
	}

	int bytes = SDLNet_TCP_Recv(clients[client], buffer, BUFFER_SIZE);
	if (bytes <= 0)
	{
		std::cout << "Client " << client << " disconnected."
		        << std::endl;

		SDLNet_TCP_DelSocket(socketSet, clients[client]);
		SDLNet_TCP_Close(clients[client]);
		clients[client] = NULL;

		socketIsFree[client] = true;
		clientCount--;

		std::cout << "Server is now connected to: " << clientCount
		        << " client(s)." << std::endl << std::endl;
	}
	else
	{
		std::cout << "Received: >>>> " << buffer
		        << " from client number: " << client << std::endl;

		for (int loop = 0; loop < MAX_CLIENTS; loop++)
		{
			//if (loop != client &&)
			//{
			//send(loop, std::string(buffer));
			//}
		}
	}
}

void Net::send(int client, Packet message)
{
        if (socketIsFree[client])
        {
                return;
        }
	//std::cout << "Sending message: " << message
	//        << " (" << message.length() << " bytes) to client number: "
	//        << client << std::endl;
	char msg[sizeof(Packet)];
	//memcpy(msg, &message, sizeof(Packet));
	message.serialize(msg);

	SDLNet_TCP_Send(clients[client], msg, sizeof(Packet));
}
