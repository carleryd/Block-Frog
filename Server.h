#ifndef SERVER_H
#define SERVER_H

#include "UDPNetwork.h"
#include "PacketParser.h"
class Shape;
class Game;

struct client
{
	client(sf::IpAddress address, unsigned short port, string name)
	{
		clientAddress = address;
		clientPort = port;
		client::name = name;
	}
	sf::IpAddress clientAddress;
	unsigned short clientPort;
	string name;
};

class Server: public  UDPNetwork
{
public:
	Server(string playerName, ShapeFactory& f, Game* game);
	~Server(void);
	void handleNewPlayer(packetInfo& pack);
	bool dropPlayer(string name); //remove player from server either if player disconnects voluntarily or not
	bool isServer() override;
	//broadcast a packet
	void broadCast(sf::Packet packet);
	//send packet to all players EXCEPT to the one with the address/port given as argument
	void broadCastExcept(sf::IpAddress address, unsigned short port, sf::Packet packet);
	vector<client*>& getClients()
	{
		return remoteConnections;
	}
	//returns nullptr if client not found
	client* getClient(string name);
private:
	bool playerMoved;
	sf::SocketSelector selector;
	vector<client*> remoteConnections;
	Game* game;
};

#endif