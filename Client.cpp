#include "Client.h"
#include <iostream>
#include <typeinfo>

Client::Client(string name, sf::IpAddress sAddress, unsigned short port, ShapeFactory& f):
	UDPNetwork(name, f), serverAddress(sAddress), serverPort(port)
{
	cout << (connect() ? "Connected to "  : "Failed to connect to " ) << sAddress.toString() << endl;
	selector.add(mySocket);
}


Client::~Client(void)
{
}

bool Client::isServer()
{
	return false;
}

bool Client::connect()
{
	sf::Packet p;
	string m = playerName;
	p << m;
	send(p, serverAddress, serverPort);
	p.clear();
	m.clear();
	int ret = receive(p);
	p >> m;
	cout << m << endl;
	return ret == sf::Socket::Done ? true : false;
}