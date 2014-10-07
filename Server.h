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
	void waitForPlayers(bool& allowJoin);
	void handleNewPlayer(packetInfo& pack);
	bool dropPlayer(string name); //remove player from server either if player disconnects voluntarily or not
	bool isServer() override;
	//broadcast a packet to all players
	void broadCast(sf::Packet packet);
	//send packet to all players EXCEPT to the one with the address/port given as argument
	void broadCastExcept(sf::IpAddress address, unsigned short port, sf::Packet packet);
	//broadcasts a list of packets
	void broadCast(vector<sf::Packet*>& packets);
	void addPlayerInfo(player_info* pi) 
	{
		playerInfos.push_back(pi);
	}
	vector<player_info*>& getPlayerInfos() 
	{
		return playerInfos;
	};
	vector<client*>& getClients()
	{
		return remoteConnections;
	}
	bool resynchTime();
private:
	bool playerMoved;
	sf::SocketSelector selector;
	vector<client*> remoteConnections;
	vector<player_info*> playerInfos;
	Game* game;
	sf::Clock synchTimer;
	sf::Time synchTime;
};

#endif