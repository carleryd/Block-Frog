#include "UDPNetwork.h"
#include <iostream>
#include "Game.h"
#include "Server.h"
#include "Client.h"
#include <memory>

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
	counter = 0;
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
	while(selector.wait(sf::seconds(0.05f)) || !exit)
	{
		if(selector.isReady(mySocket))
		{
			packetsMutex.lock();
			packets.push_front(packetInfo());
			
			if(receive(&packets.front().packet,
				packets.front().senderAddress, 
				packets.front().senderPort) != sf::Socket::Done)
				cerr << "Error when receiving data" << endl;

			//cout << "Packet size: " << packets.front().packet.getDataSize() << endl;
			packetsMutex.unlock();
		}
	}
	cout << "Listen thread joining." << endl;
}

void UDPNetwork::handleReceivedData(Game* game)
{
	while(!packets.empty())
	{
		packetsMutex.lock();
		sf::Packet* packet = &packets.front().packet;
		sf::IpAddress senderAddress = packets.front().senderAddress;
		unsigned short senderPort = packets.front().senderPort;
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
			//cout << "shape counter: " << ++counter << endl;
			if(!game->boxes.back()->getDynamic())
			{
				game->lastStaticShape = game->boxes.back();
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
					(*found)->move(info->movedir, false, info->jumped);
					(*found)->getBody()->SetLinearVelocity(info->velocity);
					(*found)->getBox()->resetUpdateClock();
					//broadcast to other players
					if(isServer())
					{
						sf::Packet p = packetParser.pack<player_info*>(PLAYER_MOVE, info);
						dynamic_cast<Server*>(this)->broadCastExcept(
							packets.front().senderAddress,
							packets.front().senderPort,
							p);
					}
				}
			}
			break;
		case PLAYER_SYNCH:
			{
				player_info* pSynch = packetParser.unpack<player_info*>(*packet);
				game->updatePlayer(pSynch);
			}
			break;
		case PLAYER_SYNCH_REQUEST:
			{
				string name = packetParser.unpack<string>(*packet);
				Player* player = game->getPlayer(name);
				if(player != nullptr)
				{
					player_info* pi = new player_info();
					pi->name = player->getName();
					pi->movedir = -1;
					pi->jumped = false;
					pi->velocity = player->getBody()->GetLinearVelocity();
					pi->position = player->getBody()->GetPosition();
					sf::Packet p = packetParser.pack<player_info*>(PLAYER_SYNCH, pi);
					send(p, senderAddress, senderPort);
				}
				else
					cout << "Player " << name << " not fond; sync not sent." << endl;
			}
			break;
		case SHAPE_SYNCH:
			{
				shared_ptr<shapeSync> s(packetParser.unpack<shapeSync*>(*packet));
				game->updateShapes(s.get());
				if(isServer())
				{
					sf::Packet broadcast = packetParser.pack(s.get());
					dynamic_cast<Server*>(this)->broadCastExcept(
						packets.front().senderAddress,
						packets.front().senderPort,
						broadcast
						);
				}
				//cout << "Synching shape " << s->shapeID << endl;
			}
			break;
		case REMOVE_SHAPE:
			{
				int id = packetParser.unpack<int>(*packet);
				game->removeShape(id);
			}
			break;
		case SHAPE_SYNCH_REQUEST:
			{
				int id = packetParser.unpack<int>(*packet);
				Shape* shape = game->getShape(id);
				if(shape != nullptr)
				{					
					shapeSync* sync = new shapeSync(*shape);

					sf::Packet p = packetParser.pack(sync);
					dynamic_cast<Server*>(this)->broadCast(p);
				}
				else
				{
					cout << "Error: could not find Shape with ID: "<< id << endl;
					sf::Packet del = packetParser.pack<int>(UDPNetwork::REMOVE_SHAPE, id);
					dynamic_cast<Server*>(this)->broadCast(del);
				}
			}
			break;
		case WATER_LEVEL:
			game->viewOffset.y = packetParser.unpack<int>(*packet);
			break;
		case START_RISE:
			game->startRise();
			break;
		case PLAYER_DEAD:
			//game->score -= 500;
			break;
		case PLAYER_RES:

			break;
		case SHAPE_STATIC:
			{
				int id = packetParser.unpack<int>(*packet);
				Shape* shape = game->getShape(id);
				if(shape != nullptr)
				{
					shape->makeStatic();
				}
			}
			break;
		case SCORE_CHANGE:
			game->score = packetParser.unpack<int>(*packet);
			break;
		case HOOK_SHOT:
			{
				hook_info* hook = packetParser.unpack<hook_info*>(*packet);
				Player* p = game->getPlayer(hook->name);
				if(p != nullptr)
				{
					p->useHook(hook->mousePos, false);
//					cout << hook->name << " uses hook" << endl;
				}
				else
					cout << hook->name << " not found" << endl;
				delete hook;
			}
			break;
		case HOOK_AIM:
			{
				hook_info* hook = packetParser.unpack<hook_info*>(*packet);
				Player* p = game->getPlayer(hook->name);
				if(p != nullptr)
				{
					p->aimHook(hook->mousePos + game->getViewOffset(), false);
//					cout << hook->name << " aims hook" << endl;
				}
				else
                    cout << hook->name << " not found" << endl;
				delete hook;
			}
			break;
		default:
			cerr << "Type " << type << " is not a recognized data type!" << endl;
			break;
		}
		packets.pop_front();
		packetsMutex.unlock();
	}
}