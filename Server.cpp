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

void Server::waitForPlayers(bool& allowJoin)
{
	string m;
	sf::Packet packet;
	sf::IpAddress remoteAddress;
	unsigned short remotePort;

	while(selector.wait() && allowJoin) //waits infinitely long now...
	{
		if(selector.isReady(mySocket))
		{
			receive(&packet, remoteAddress, remotePort);
			if(!(packet >> m))
				cerr <<	"ERROR" << endl; 
			remoteConnections.push_back(new client(remoteAddress, remotePort, m));
			packet.clear();
			cout << m << " has joined." << endl;
			m = "Welcome to " + playerName + "'s server.";
			packet << m;
			send(packet, remoteAddress, remotePort);
			//ask if enough players
			//break;
		}
	}
}

void Server::broadCast(Shape* s)
{
	if(s == nullptr)
		return;
	for(unsigned i = 0; i < remoteConnections.size(); i++)
	{
        sf::Packet tempPackage = packetParser.packageShape(s);
		send(tempPackage, remoteConnections[i]->clientAddress, remoteConnections[i]->clientPort);
		cout << "Broadcast!" << endl;
	}
}

void Server::dropPlayer()
{

};