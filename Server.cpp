#include "Server.h"
#include <iostream>

Server::Server(string playerName, ShapeFactory& f):
	UDPNetwork(playerName, f)
{
	selector.add(mySocket);
}


Server::~Server(void)
{
	while(!remoteConnections.empty())
	{
		delete remoteConnections.back();
		remoteConnections.pop_back();
	}
}

bool Server::isServer()
{
	return true;
}

void Server::waitForPlayers()
{
	string m;
	sf::Packet packet;
	sf::IpAddress remoteAddress;
	unsigned short remotePort;

	while(selector.wait()) //waits infinitely long now...
	{
		if(selector.isReady(mySocket))
		{
			receive(packet, remoteAddress, remotePort);
			if(!(packet >> m))
				cerr <<	"ERROR" << endl; 
			cout << m << " has joined." << endl;
			remoteConnections.push_back(new client(remoteAddress, remotePort, m));
			packet.clear();
			m = "Welcome to " + playerName + "'s server.";
			packet << m;
			send(packet, remoteAddress, remotePort);
			//ask if enough players
			break;
		}
	}
}

void Server::broadCast(Shape* s)
{
	for(unsigned i = 0; i < remoteConnections.size(); i++)
	{
		send(packetParser.packageShape(s), remoteConnections[i]->clientAddress, remoteConnections[i]->clientPort);
	}
}