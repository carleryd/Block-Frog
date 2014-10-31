#ifndef CLIENT_H
#define CLIENT_H

#include "UDPNetwork.h"
#include <list>
using namespace std;

class Game;

class Client: public UDPNetwork
{
public:
	Client(Game* game, sf::IpAddress serverAddress, unsigned short port);
	~Client(void);
	bool isServer() override;
	bool connect();
	void sendToServer(sf::Packet& packet);
	void dropServer();
private:
	
	sf::IpAddress serverAddress;
	unsigned short serverPort;
};

#endif