#ifndef SERVER_H
#define SERVER_H

#include "UDPNetwork.h"
#include "PacketParser.h"
class Shape;

struct client
{
	client(sf::IpAddress address, unsigned short port, string name)
	{
		clientAddress = address;
		port = clientPort;
	}
	sf::IpAddress clientAddress;
	unsigned short clientPort;
	string name;
};

class Server: public  UDPNetwork
{
public:
	Server(string playerName, ShapeFactory& f);
	~Server(void);
	void waitForPlayers();
	bool isServer() override;
	void broadCast(Shape* s);
private:
	sf::SocketSelector selector;
	vector<client*> remoteConnections;
};

#endif