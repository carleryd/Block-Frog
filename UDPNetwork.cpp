#include "UDPNetwork.h"
#include <iostream>
#include "Game.h"
#include "Server.h"

UDPNetwork::UDPNetwork(string PlayerName, ShapeFactory& factory):
	packetParser(factory)
{
	if(mySocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
		cerr << "Could not bind port" << endl;
	else
	{
		cout << "My IP address: "<< myAddress.getPublicAddress().toString() << endl;
		cout << "My port: " << mySocket.getLocalPort() << endl; 
	}
	playerName = PlayerName;
	selector.add(mySocket);
	mySocket.setBlocking(true);
}

UDPNetwork::~UDPNetwork(void)
{
}

int UDPNetwork::send(sf::Packet p, sf::IpAddress& a, unsigned short& port)
{
	//cout << "Packet sent." << endl;
	return mySocket.send(p, a, port);
}

int UDPNetwork::receive(sf::Packet* p, sf::IpAddress& a, unsigned short& port)
{
	//cout << "Packet received from " << a.getPublicAddress().toString() << " at port "<< port << endl;
	int r = mySocket.receive(*p, a, port);
	if(port == mySocket.getLocalPort() && a == sf::IpAddress::getPublicAddress())
	{
		cout << "FATAL ERROR: SERVER AND CLIENT USING SAME PORT" << endl;
		cout << "Trying to recover by removing from remote connections list" << endl;
	}
	return r;
}

int UDPNetwork::receive(sf::Packet* p)
{
	unsigned short port;
    sf::IpAddress pointlessIP;
    
	return receive(p, pointlessIP, port);
}

void UDPNetwork::listen()
{
	while(selector.wait())
	{
		if(selector.isReady(mySocket) && !packetsOccupied)
		{
			packetsOccupied = true;
			packets.push_front(packetInfo());
			
			if(receive(&packets.front().packet,
				packets.front().senderAddress, 
				packets.front().senderPort) != sf::Socket::Done)
				cerr << "Error when receiving data" << endl;
			//packets.front()->
			cout << "Packet size: " << packets.front().packet.getDataSize() << endl;
			packetsOccupied = false;
		}
	}
}

void UDPNetwork::handleReceivedData(Game* game)
{
	while(!packets.empty() && !packetsOccupied)
	{
		packetsOccupied = true;
		sf::Packet* packet = &packets.front().packet;
		int type;
		*packet >> type;
		switch (type)
		{
		case SERVER_EXIT:
			//game->getWindow()->close();
			break;
		case CLIENT_EXIT:
			break;
		case NEW_PLAYER:
		{
			game->addRemotePlayer(new Player(game));
			b2Vec2* newpos = packetParser.unpack<b2Vec2>(*packet);
			game->remotePlayers.back()->setPosition( newpos);
			if(isServer())
			{
				dynamic_cast<Server*>(this)->handleNewPlayer(packets.front());
			}
		}
			break;
		case SHAPE:
			game->boxes.push_back(packetParser.unpack<Shape>(*packet));
			break;
		case PLAYER_MOVE:
			{
				cout << "moving player" << endl;
				typedef list<Player*> players;
				typedef list<Player*>::iterator itr;
				player_info* info = packetParser.unpack<player_info>(*packet);
				string& name = info->name;
				players remotePlayers = game->getRemotePlayers();
				//find player that has made a move
				itr found = std::find_if(remotePlayers.begin(), remotePlayers.end(), 
					[name](Player* p)
					{
						return p->getName() == name;
					});
				//player found
				if(found != remotePlayers.end())
				{
					//move player
					(*found)->move(info->movedir, false);
					if(isServer())
					{
						dynamic_cast<Server*>(this)->addPlayerInfo(info);
					}
				}
			}
			break;
		default:
			cerr << "Type " << type << " is not a recognized data type!" << endl;
			break;
		}
		packets.pop_front();
		packetsOccupied = false;
	}
}