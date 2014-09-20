#include "UDPNetwork.h"
#include <iostream>

UDPNetwork::UDPNetwork(void)
{
	if(mySocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
		cerr << "Could not bind port" << endl;
}


UDPNetwork::~UDPNetwork(void)
{
}

int UDPNetwork::send(sf::Packet& p, sf::IpAddress a, unsigned short port)
{
	return mySocket.send(p, a, port);
}

int UDPNetwork::receive(sf::Packet& p, sf::IpAddress a, unsigned short port)
{
	return mySocket.receive(p, a, port);
}
