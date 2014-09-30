#include "Server.h"
#include <iostream>
#include "Game.h"
Server::Server(string playerName, ShapeFactory& f, Game* g):
	UDPNetwork(playerName, f), game(g)
{
	selector.add(mySocket);
	playerMoved = false;
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

void Server::handleNewPlayer(packetInfo& pack)
{
	string m;
	pack.packet >> m;;
	cout << m << " has joined." << endl;
	remoteConnections.push_back(new client(sf::IpAddress(pack.senderAddress), pack.senderPort, m));
	
	//tell player connection was successful
	sf::Packet sendPack;
	m = "Welcome to " + playerName + "'s game.";
	sendPack << m;
	send(sendPack, pack.senderAddress, pack.senderPort);

	//send other to players' Frogs to newly connected player
	typedef list<Player*>::iterator iter;
	list<Player*>& players = game->getRemotePlayers();
	for(iter i = players.begin(); i != players.end(); i++)
	{
		sf::Packet broadcastPack;
		broadcastPack << UDPNetwork::NEW_PLAYER;
		broadcastPack << (*i)->getPosition()->x << (*i)->getPosition()->y;
		broadcastPack << (*i)->getName();
		broadCast(sendPack);
	}
	//...and don't forget the host's frog! ;)
	sf::Packet hostfrog;
	hostfrog << UDPNetwork::NEW_PLAYER;
	hostfrog << game->getPlayer()->getPosition()->x << game->getPlayer()->getPosition()->y;
	hostfrog << game->getPlayer()->getName();
	send(hostfrog, pack.senderAddress, pack.senderPort);

	//send connected player's Frog to other players
	sf::Packet broadcastPack;
	broadcastPack << UDPNetwork::NEW_PLAYER;
	broadcastPack << game->getRemotePlayers().back()->getPosition()->x << game->getRemotePlayers().back()->getPosition()->y;
	broadcastPack << game->getRemotePlayers().back()->getName();
	broadCastExcept(pack.senderAddress, pack.senderPort, broadcastPack);

		
}

void Server::broadCast(sf::Packet& packet)
{
	for(unsigned i = 0; i < remoteConnections.size(); i++)
	{
		send(packet, remoteConnections[i]->clientAddress, remoteConnections[i]->clientPort);
		cout << "Broadcast!" << endl;
	}
}

void Server::broadCastExcept(sf::IpAddress address, unsigned short port, sf::Packet& packet)
{
	for(unsigned i = 0; i < remoteConnections.size(); i++)
	{
		if(remoteConnections[i]->clientAddress != address && remoteConnections[i]->clientPort != port)
		{
			send(packet, remoteConnections[i]->clientAddress, remoteConnections[i]->clientPort);
			cout << "Broadcast!" << endl;
		}
	}
}