#include "Server.h"
#include <iostream>
#include "Game.h"
#include "PacketParser.h"
Server::Server(string playerName, ShapeFactory& f, Game* g):
	UDPNetwork(playerName, f), game(g)
{
	selector.add(mySocket);
	playerMoved = false;
	synchTimer.restart();
	synchTime = sf::seconds(1);
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
	game->getRemotePlayers().back()->setName(m);

	//tell player connection was successful
	sf::Packet sendPack;
	m = "Welcome to " + playerName + "'s game.";
	sendPack << m;
	send(sendPack, pack.senderAddress, pack.senderPort);

	//send other to players' Frogs to newly connected player
	typedef list<Player*>::iterator iter;
	list<Player*>& players = game->getRemotePlayers();
	for(iter i = players.begin(); i != --players.end(); i++)
	{
		sf::Packet newPlayerPacket = packetParser.pack(*i);
		send(newPlayerPacket, remoteConnections.back()->clientAddress, remoteConnections.back()->clientPort);
	}
	//...and don't forget the host's frog! ;)
	sf::Packet hostfrog = packetParser.pack(game->getPlayer());
	send(hostfrog, pack.senderAddress, pack.senderPort);

	//send the shapes to the newly connected player
	vector<Shape*> shapes = game->getShapes();
	client* newClient = remoteConnections.back();
	PacketParser* pp = &packetParser;
	Server* server = this;
	//++shapes.begin() so that the ground isn't sent
	for_each(++shapes.begin(), shapes.end(), [newClient, pp, server](Shape* shape)
	{
		sf::Packet packet = pp->pack(shape, UDPNetwork::SHAPE);
		server->send(packet, newClient->clientAddress, newClient->clientPort);
	});

	//send connected player's Frog to other players
	sf::Packet broadcastPack = packetParser.pack(game->getRemotePlayers().back());
	broadCastExcept(pack.senderAddress, pack.senderPort, broadcastPack);

		
}

void Server::broadCast(sf::Packet packet)
{
	for(unsigned i = 0; i < remoteConnections.size(); i++)
	{
		send(packet, remoteConnections[i]->clientAddress, remoteConnections[i]->clientPort);
		//cout << "Broadcast!" << endl;
	}
}

void Server::broadCastExcept(sf::IpAddress address, unsigned short port, sf::Packet packet)
{
	for(unsigned i = 0; i < remoteConnections.size(); i++)
	{
		if(remoteConnections[i]->clientAddress != address && remoteConnections[i]->clientPort != port)
		{
			send(packet, remoteConnections[i]->clientAddress, remoteConnections[i]->clientPort);
			//cout << "Broadcast!" << endl;
		}
	}
}

void Server::broadCast(vector<sf::Packet*>& packets)
{
	for(unsigned i = 0; i < remoteConnections.size(); i++)
	{
		for(unsigned j = 0; j < packets.size(); ++j)
		{
			send(*packets[j], remoteConnections[i]->clientAddress, remoteConnections[i]->clientPort);
		}
	}
}

bool Server::dropPlayer(string name)
{
	vector<client*>::iterator i = remove_if(remoteConnections.begin(), remoteConnections.end(), [name](client* a)
	{
		return name == a->name;
	});
	if(i != remoteConnections.end())
	{
		delete *i;
		remoteConnections.erase(i);
		return true;
	}
	else
	{
		cout << name << " could not be found among remote connections." << endl;
		return false;
	}
}

bool Server::resynchTime()
{
	if(synchTimer.getElapsedTime() > synchTime)
	{
		synchTimer.restart();
		return true;
	}
	else 
		return false;
}