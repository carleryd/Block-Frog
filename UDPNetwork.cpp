#include "UDPNetwork.h"
#include <iostream>
#include "Game.h"
#include "Server.h"
#include "Client.h"
#include "Lobby.h"
#include "Director.h"
#include <memory>

UDPNetwork::UDPNetwork(Game* game):
	packetParser(game)
{
	exit = false;
	if(mySocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
        //if(mySocket.bind(49977) != sf::Socket::Done)
		cerr << "Could not bind port" << endl;
	else
	{
		cout << "My IP address: "<< myAddress.getPublicAddress().toString() << endl;
		cout << "My port: " << mySocket.getLocalPort() << endl;
	}
//	playerName = PlayerName;
//    playerID = playerID_;
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
    sf::IpAddress pointlessIP;
	unsigned short port;
    
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
				int localID;
				*packet >> localID;
				Server* server = dynamic_cast<Server*>(this);
				vector<client*>& clients = server->getClients();
                
				//tell everyone else that a client has disconnected
				if(isServer())
				{
					for_each(clients.begin(), clients.end(), [localID, server](client* c)
                             {
                                 sf::Packet p;
                                 p << CLIENT_EXIT;
                                 p << c->localID;
                                 server->broadCast(p);
                             });
				}
				if(game->removeRemotePlayer(localID))
					cout << localID << " has disconnected." << endl;
				else
					cout << "failed to disconnect " << localID << endl;
			}
                break;
            case NEW_PLAYER:
			{
				dynamic_cast<Server*>(this)->playerEnteredLobby(packets.front());
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
                int& localID = info->localID;
				players remotePlayers = game->getRemotePlayers();
				//find player that has made a move
				itr found = std::find_if(remotePlayers.begin(), remotePlayers.end(),
                                         [localID](Player* p)
                                         {
                                             return p->getLocalID() == localID;
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
				int localID = packetParser.unpack<int>(*packet);
				Player* player = game->getPlayer(localID);
				if(player != nullptr)
				{
					player_info* pi = new player_info();
					pi->localID = player->getLocalID();
					pi->movedir = -1;
					pi->jumped = false;
					pi->velocity = player->getBody()->GetLinearVelocity();
					pi->position = player->getBody()->GetPosition();
					sf::Packet p = packetParser.pack<player_info*>(PLAYER_SYNCH, pi);
					send(p, senderAddress, senderPort);
				}
				else
					cout << "Player " << localID << " not fond; sync not sent." << endl;
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
            {
                int waterLevel = packetParser.unpack<int>(*packet);
				game->viewOffset.y = waterLevel;
                //                window->setView(*view);
                //                cout << "Incoming WATER_LEVEL: " << waterLevel << endl;
        	}
                break;
            case START_RISE:
                game->startRise();
                break;
            case PLAYER_DEAD:
                //game->score -= 500;
                break;
            case PLAYER_RES:
            {
            	res_info* player = packetParser.unpack<res_info*>(*packet);
            	game->getPlayer(player->localID)->resetPlayer(&player->spawn);
        	}
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
            case PLATFORM_CHANGE:
            {
                int id = packetParser.unpack<int>(*packet);
                game->setStaticPlatform(game->getShape(id));
            }
                break;
            case PREPTIME_CHANGE:
            {
                float elapsedPrepTime = packetParser.unpack<int>(*packet);
                game->setElapsedPrepTime(elapsedPrepTime);
            }
                break;
            case PREPTIME_OVER:
            {
                bool gameHasStarted = packetParser.unpack<bool>(*packet);
                game->setGameHasStarted(gameHasStarted);
            }
                break;
            case PREPTIME_OVER_REQUEST:
            {
                sf::Packet packet = packetParser.pack(UDPNetwork::PREPTIME_OVER);
                packet << game->getGameHasStarted();
                dynamic_cast<Server*>(this)->broadCast(packet);
            }
                break;
            case SCORE_CHANGE:
                game->score = packetParser.unpack<int>(*packet);
                break;
            case HOOK_SHOT:
			{
				hook_info* hook = packetParser.unpack<hook_info*>(*packet);
				Player* p = game->getPlayer(hook->localID);
				if(p != nullptr)
				{
					p->useHook(hook->mousePos, false);
                    //					cout << hook->name << " uses hook" << endl;
				}
				else
					cout << hook->localID << " not found" << endl;
				delete hook;
			}
                break;
            case HOOK_AIM:
			{
				hook_info* hook = packetParser.unpack<hook_info*>(*packet);
				Player* player = game->getPlayer(hook->localID);
				if(player != nullptr)
				{
                    //                    cout << "INCOMING: " << hook->mousePos.y + game->getViewOffset().y << endl;
					player->aimHook(hook->mousePos/* + game->getViewOffset()*/, false);
                    //					cout << hook->name << " aims hook" << endl;
				}
				else
                    cout << hook->localID << " not found" << endl;
				delete hook;
			}
                break;
            case HOOK_RELEASE:
			{
				hook_info* hook = packetParser.unpack<hook_info*>(*packet);
				Player* p = game->getPlayer(hook->localID);
				if(p != nullptr)
				{
					p->releaseHook(false);
				}
				else
                    cout << hook->localID << " not found" << endl;
				delete hook;
			}
                break;
            case CONNECTED_PLAYERS_REQUEST:
            {
                cout << "ON SERVER: client is requesting connected players" << endl;
                vector<LobbyPlayer>& lobbyPlayers = game->getLobby()->getConnected();
                // I use this to find the lowest free ID. I could just get lobbyPlayers.size()+1
                // but that way it would get incorrect(color for example) if people left
                int lowestFreeID = 1;
                for(LobbyPlayer connected : lobbyPlayers) {
                    lp_info* lp = new lp_info();
                    lp->ID = connected.ID;
                    if(lp->ID == lowestFreeID) lowestFreeID++;
                    lp->alias = connected.alias;
                    lp->color = connected.color;
                    
                    sf::Packet packet = packetParser.pack(lp);
                    send(packet, senderAddress, senderPort);
                }
                
                // Add the client which just joined and requested the info
                lp_info* requester = new lp_info();
                int latestLocalID = lowestFreeID;
                requester->ID = latestLocalID;
                requester->alias = "client";
                requester->color = game->getLobby()->getColors()[lobbyPlayers.size()];
                sf::Packet packet = packetParser.pack(requester);
                dynamic_cast<Server*>(this)->broadCast(packet);
                
                packet.clear();
                packet = packetParser.pack<int>(UDPNetwork::LOCAL_ID, latestLocalID);
                send(packet, senderAddress, senderPort);

                LobbyPlayer requesterPlayer(requester->ID, requester->alias, requester->color);
                game->getLobby()->addPlayer(requesterPlayer);
            }
                break;
            case NEW_LOBBY_PLAYER:
            {
                cout << "ON CLIENT: server is sending a new lobby player" << endl;
                lp_info* lpi = packetParser.unpack<lp_info*>(*packet);
                LobbyPlayer lobbyPlayer(lpi->ID, lpi->alias, lpi->color);
				game->getLobby()->addPlayer(lobbyPlayer);
            }
                break;
            case GAME_STARTED:
            {
                game->getDirector()->setState(GAME);
            }
                break;
            case LOCAL_ID:
            {
                int incLocalID = packetParser.unpack<int>(*packet);
                cout << "ON CLIENT: INCOMING LOCAL ID: " << incLocalID << endl;
                game->setLocalID(incLocalID);
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