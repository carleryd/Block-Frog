#include "Client.h"
#include <iostream>
#include <typeinfo>

Client::Client(string name, sf::IpAddress sAddress, unsigned short port, ShapeFactory& f):
	UDPNetwork(name, f), serverAddress(sAddress), serverPort(port)
{
	//cout << (connect() ? "Connected to "  : "Failed to connect to " ) << sAddress.toString() << endl;
	selector.add(mySocket);
}


Client::~Client(void)
{
}

bool Client::isServer()
{
	return false;
}

bool Client::connect(b2Vec2* pPos)
{
	//send
	sf::Packet p;
	p << UDPNetwork::NEW_PLAYER;
	p << pPos->x << pPos->y;
	string m = playerName;
	p << m;
	send(p, serverAddress, serverPort);
	p.clear();
	m.clear();
	
	//receive
	int ret = receive(&p);
	p >> m;
	
	cout << "Message received from host: " << m << endl;
	return ret == sf::Socket::Done ? true : false;
}
void Client::sendToServer(sf::Packet& p)
{
	send(p, serverAddress, serverPort);
}

void Client::dropServer()
{
	serverAddress = NULL;
	serverPort = 0;
	cout << "Server dropped." << endl;
}