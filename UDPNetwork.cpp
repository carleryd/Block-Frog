#include "UDPNetwork.h"
#include <iostream>
#include "Game.h"
#include "Server.h"
#include "Client.h"

UDPNetwork::UDPNetwork(string PlayerName, ShapeFactory& factory):
	packetParser(factory)
{
	exit = false;
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
	numberOfShapes = 0;
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
	while(selector.wait(sf::seconds(0.05)) || !exit)
	{
		if(selector.isReady(mySocket) && !packetsOccupied)
		{
			packetsOccupied = true;
			packets.push_front(packetInfo());
			
			if(receive(&packets.front().packet,
				packets.front().senderAddress, 
				packets.front().senderPort) != sf::Socket::Done)
				cerr << "Error when receiving data" << endl;

			//cout << "Packet size: " << packets.front().packet.getDataSize() << endl;
			packetsOccupied = false;
		}
	}
	cout << "Listen thread joining." << endl;
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
			//dynamic_cast<Client*>(this)->dropServer();  //DOESN'T WORK ATM
			cout << "Server has disconnected." << endl;
			game->exitGame();
			break;
		case CLIENT_EXIT:
			{
				string name;
				*packet >> name;
				Server* server = dynamic_cast<Server*>(this);
				vector<client*>& clients = server->getClients();

				//tell everyone else that a client has disconnected
				if(isServer())
				{
					for_each(clients.begin(), clients.end(), [name, server](client* a)
					{
						sf::Packet p;
						p << CLIENT_EXIT;
						p << a->name;
						server->broadCast(p);
					});
				}
				if(game->removeRemotePlayer(name))
					cout << name << " has disconnected." << endl;
				else
					cout << "failed to disconnect " << name << endl;
			}
			break;
		case NEW_PLAYER:
		{
			
			b2Vec2* newpos = packetParser.unpack<b2Vec2*>(*packet);
			game->addRemotePlayer(new Player(game));
			game->remotePlayers.back()->setPosition( newpos);
			
			if(isServer())
			{
				dynamic_cast<Server*>(this)->handleNewPlayer(packets.front());
			}
			else
				game->getRemotePlayers().back()->setName(packetParser.unpack<string>(*packet));
		}
			break;
		case SHAPE:
			game->boxes.push_back(packetParser.unpack<Shape*>(*packet));
			break;
		case SHAPE_SYNCH:
			{
				vector<Shape*>& shapes =  game->getShapes();
				//don't compare ground (1st element).  
				numberOfShapes == 0 ? ++numberOfShapes % shapes.size(): numberOfShapes;

				syncStruct s;
				b2Vec2 size;
				*packet >> s.position.x >> s.position.y;
				*packet >> size.x >> size.y;
				*packet >> s.velocity.x >> s.velocity.y;
				*packet >> s.angle >> s.angularVelocity;


				//if NOT in synch replace old shape with serverShape
				if(!(*shapes[numberOfShapes] == s))
				{
					cout << "SYNCHING" << endl;
				}
			}
			break;
		case PLAYER_MOVE:
			{
				//cout << "moving player" << endl;
				typedef list<Player*> players;
				typedef list<Player*>::iterator itr;
				player_info* info = packetParser.unpack<player_info*>(*packet);
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