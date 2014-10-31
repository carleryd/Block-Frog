#ifndef SERVER_H
#define SERVER_H

#include "UDPNetwork.h"
#include "PacketParser.h"
class Shape;
class Game;

struct client
{
	client(int localID_, sf::IpAddress address, unsigned short port)
	{
        localID = localID_;
		clientAddress = address;
		clientPort = port;
	}
	int localID;
	sf::IpAddress clientAddress;
	unsigned short clientPort;
};

class Server: public  UDPNetwork
{
public:
	Server(Game* game);
	~Server(void);
	void handleNewPlayer(packetInfo& pack);
	//returns nullptr if client not found
	client* getClient(int localID);
    //remove player from server either if player disconnects voluntarily or not
	bool dropPlayer(int localID);
    
  	void playerEnteredLobby(packetInfo& pack);
    void playerLeftLobby(int ID); // not yet implemented
    
	bool isServer() override;
	//broadcast a packet
	void broadCast(sf::Packet packet);
	//send packet to all players EXCEPT to the one with the address/port given as argument
	void broadCastExcept(sf::IpAddress address, unsigned short port, sf::Packet packet);
	vector<client*>& getClients()
	{
		return remoteConnections;
	}
private:
	Game* game;
    
	sf::SocketSelector selector;
	vector<client*> remoteConnections;
	bool playerMoved;
};

#endif